#include "../pch.h"
#include "Input.h"

shared_ptr<Joystick> joysticks[3];
vector<shared_ptr<Joystick>> tempJoysticks;

bool endNow = false;

Input::Input() {

}


Input::~Input() {

	ReleaseCapture();
	ShowCursor(true);
}

#include "../Engine/GameEngine.h"
bool Input::initRawInput(HWND hwnd) {

	keys = make_unique<Keyboard>();
	mouse = make_shared<MouseController>(hwnd);
	if (guiFactory != NULL)
		mouse->loadMouseIcon(guiFactory.get(), "Mouse Icon");

	return true;
}



ControllerListener::ControllerListener() {

	joysticks[0] = make_shared<Joystick>(0);
	joysticks[1] = make_shared<Joystick>(1);
	joysticks[2] = make_shared<Joystick>(2);
	availableControllerSlots.push_back(0);
	availableControllerSlots.push_back(1);
	availableControllerSlots.push_back(2);
}

ControllerListener::~ControllerListener() {

	endNow = true;
	joystickMap.clear();

	std::deque<int> empty;
	swap(availableControllerSlots, empty);
}


void ControllerListener::addJoysticks(vector<HANDLE> handles) {

	if (handles.size() < joystickMap.size() + unclaimedJoysticks.size()) {
		// joystick was removed it - find it!
		HANDLE found = NULL;
		shared_ptr<Joystick> foundJoy;
		for (const auto& joyDev : joystickMap) {
			if (matchFound(handles, joyDev.first))
				continue;

			foundJoy = joyDev.second;
			found = joyDev.first;
			break;
		}
		while (found) {

			size_t oldSize = joystickMap.size();

			map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
			mapIt = joystickMap.find(found);
			if (mapIt != joystickMap.end()) {

				joystickMap.erase(mapIt);

				if (joystickMap.size() < oldSize) {

					availableControllerSlots.push_back(foundJoy->slot);
					foundJoy->registerNewHandle(NULL);

					OutputDebugString(L"Joystick removed\n");
					controllerRemoved(foundJoy->slot);
					guiOverlay->controllerRemoved(foundJoy->slot);
				}
			}
			found = NULL;

			for (const auto& joyDev : joystickMap) {
				if (matchFound(handles, joyDev.first))
					continue;
				foundJoy = joyDev.second;
				found = joyDev.first;
				break;
			}
		}

		for (const auto& unclaimed : unclaimedJoysticks) {
			if (matchFound(handles, unclaimed.first))
				continue;
			foundJoy = unclaimed.second;
			found = unclaimed.first;
			break;

		}

		while (found) {

			map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
			mapIt = unclaimedJoysticks.find(found);
			if (mapIt != unclaimedJoysticks.end()) {
				foundJoy->slot = SLOT_OF_DEATH; // slot sixty nine is the slot of death!
				unclaimedJoysticks.erase(mapIt);
			}

			found = NULL;

			for (const auto& unclaimed : unclaimedJoysticks) {
				if (matchFound(handles, unclaimed.first))
					continue;
				foundJoy = unclaimed.second;
				found = unclaimed.first;
				break;
			}
		}

	} else if (handles.size() > joystickMap.size()) {
		for (const auto& newHandle : handles) {
			if (joystickMap.find(newHandle) != joystickMap.end()) {
				//OutputDebugString(L"That joystick already registered.\n");
			} else if (gameInitialized) {
				if (availableControllerSlots.size() == 0) {
					// too many players
					return;
				}

				// create joystick and wait for player response
				if (unclaimedJoysticks.find(newHandle) == unclaimedJoysticks.end()) {
					shared_ptr<Joystick> newJoy = make_shared<Joystick>(10);
					newJoy->registerNewHandle(newHandle);
					unclaimedJoysticks[newHandle] = newJoy;
					tempJoysticks.push_back(newJoy);

					JoyData* data = new JoyData(newJoy);
					DWORD id;
					threadHandles.push_back(
						CreateThread(NULL, 0, waitForPlayerThread, (PVOID) data, 0, &id));

				}
			} else {
				USHORT nextAvailableSlot = availableControllerSlots[0];
				availableControllerSlots.pop_front();

				shared_ptr<Joystick> newStick = joysticks[nextAvailableSlot];
				newStick->registerNewHandle(newHandle);
				joystickMap[newHandle] = newStick;


				OutputDebugString(L"New joystick found!\n");

				// Create a thread to ping the HUD until it's ready
				JoyData* data = new JoyData(newStick);
				DWORD id;
				threadHandles.push_back(
					CreateThread(NULL, 0, waitForHUDThread, (PVOID) data, 0, &id));
			}
		}
	}
}



void ControllerListener::parseRawInput(PRAWINPUT pRawInput) {

	HANDLE handle = pRawInput->header.hDevice;
	if (joystickMap.find(handle) != joystickMap.end()) {
		joystickMap[handle]->parseRawInput(pRawInput);
	} else if (unclaimedJoysticks.find(handle) != unclaimedJoysticks.end()) {
		unclaimedJoysticks[handle]->parseRawInput(pRawInput);
	}
}

bool ControllerListener::matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle) {

	for (HANDLE newHandle : newHandles)
		if (newHandle == joystickHandle)
			return true;
	return false;
}


DWORD WINAPI waitForPlayerThread(PVOID pVoid) {

	JoyData* joyData = (JoyData*) pVoid;

	int tempSlot = guiOverlay->controllerWaiting(joyData->joystick);
	while (joyData->joystick->slot >= 3) {

		if (joyData->joystick->slot == SLOT_OF_DEATH) {
			guiOverlay->controllerRemoved(tempSlot);
			delete joyData;
			return 0;
		}
		if (endNow) {

			delete joyData;
			return 0;
		}
		Sleep(500);
	}

	joyData->joystick->slot = tempSlot;
	guiOverlay->controllerAccepted(joyData->joystick);
	delete joyData;
	return 0;

}


DWORD WINAPI waitForHUDThread(PVOID pVoid) {

	JoyData* data = (JoyData*) pVoid;

	while (guiOverlay == NULL) {
		// Jus hol up
		//wostringstream wss;
		//wss << L"Test Thread #" << GetCurrentThreadId() << endl;
		//wss << data->joystick->slot << " waiting for HUD." << endl;
		//OutputDebugString(wss.str().c_str());
		
		if (endNow) {
			delete data;
			return 0;
		}
		Sleep(500);
	}
	wostringstream ws;
	ws << L"Player " << (data->joystick->slot + 1);
	guiOverlay->setDialogText(data->joystick->slot, ws.str());
	delete data;
	return 0;
}


void ControllerListener::controllerAccepted(HANDLE handle) {

	shared_ptr<Joystick> joy = joysticks[availableControllerSlots[0]];
	swap(availableControllerSlots[0], availableControllerSlots.back());
	availableControllerSlots.pop_back();
	joy->registerNewHandle(handle);
	joystickMap[handle] = joy;

	guiOverlay->setDialogText(joy->slot, L"Hello!"); // This player becomes player 1

	map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
	mapIt = unclaimedJoysticks.find(handle);
	if (mapIt != unclaimedJoysticks.end())
		unclaimedJoysticks.erase(mapIt);
}

