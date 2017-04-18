#include "../pch.h"
#include "Input.h"

shared_ptr<Joystick> joysticks[3];
vector<shared_ptr<Joystick>> tempJoysticks;

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
	//lostDevices.clear();
	joystickMap.clear();
	availableControllerSlots.clear();
}


void ControllerListener::addJoysticks(vector<HANDLE> handles) {

	if (handles.size() < joystickMap.size()) {
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
					//lostDevices.push_back(foundJoy);

					OutputDebugString(L"Joystick removed\n");
					controllerRemoved(foundJoy->slot);
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
	} else if (handles.size() > joystickMap.size()) {
		for (const auto& newHandle : handles) {
			if (joystickMap.find(newHandle) != joystickMap.end()) {
				OutputDebugString(L"That joystick already registered.\n");
			} else if (gameInitialized) {
				// create joystick and wait for player response
				if (unclaimedJoysticks.find(newHandle) == unclaimedJoysticks.end()) {
					shared_ptr<Joystick> newJoy = make_shared<Joystick>(10);
					newJoy->registerNewHandle(newHandle);
					unclaimedJoysticks[newHandle] = newJoy;
					tempJoysticks.push_back(newJoy);
					//newController(newHandle);
				}
			} else {
				USHORT nextAvailableSlot = availableControllerSlots[0];
				swap(availableControllerSlots[0], availableControllerSlots.back());
				availableControllerSlots.pop_back();

				shared_ptr<Joystick> newStick = joysticks[nextAvailableSlot];
				newStick->registerNewHandle(newHandle);
				joystickMap[newHandle] = newStick;


				OutputDebugString(L"New joystick found!\n");

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


void ControllerListener::controllerAccepted(HANDLE handle) {

	shared_ptr<Joystick> joy = joysticks[availableControllerSlots[0]];
	swap(availableControllerSlots[0], availableControllerSlots.back());
	availableControllerSlots.pop_back();
	joy->registerNewHandle(handle);
	joystickMap[handle] = joy;

	guiOverlay->setDialogText(joy->slot, L"Hello!");

	map<HANDLE, shared_ptr<Joystick>>::iterator mapIt;
	mapIt = unclaimedJoysticks.find(handle);
	if (mapIt != unclaimedJoysticks.end())
		unclaimedJoysticks.erase(mapIt);
}

