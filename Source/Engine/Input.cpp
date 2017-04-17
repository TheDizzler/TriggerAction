#include "../pch.h"
#include "Input.h"


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


void Input::addJoysticks(vector<HANDLE> handles) {

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

					bool foundI = false;
					int i;
					for (i = 0; i < joysticks.size(); ++i) {
						if (joysticks[i].get() == foundJoy.get()) {
							foundI = true;
							break;
						}
					}
					if (foundI) {
						lostDevices.push_back(foundJoy);
						joysticks.erase(joysticks.begin() + i);
					}
					OutputDebugString(L"Joystick removed\n");
				}
			}
			found = NULL;

			for (const auto& joyDev : joystickMap) {
				if (matchFound(handles, joyDev.first))
					continue;
				lostDevices.push_back(joyDev.second);
				found = joyDev.first;
				break;
			}

		}
	} else if (handles.size() > joystickMap.size()) {
		for (const auto& newHandle : handles) {
			if (joystickMap.find(newHandle) != joystickMap.end()) {
				OutputDebugString(L"That joystick already registered.\n");
			} else if (lostDevices.size() > 0) {
				// create joystick and wait for player response


			} else {
				shared_ptr<Joystick> newStick = make_shared<Joystick>(newHandle, ++numSticks);
				joystickMap[newHandle] = newStick;
				joysticks.push_back(newStick);
				OutputDebugString(L"New joystick found!\n");

			}
		}
	}
}


void Input::controllerRemoved(PDEV_BROADCAST_DEVICEINTERFACE removedDevice) {

	/*for (const auto& joy : joystickDeviceMap) {
		if (joy.first->dwType == removedDevice->dbcc_devicetype) {
			OutputDebugString(L"Found removed joystick");
		}
	}*/


}


void Input::parseRawInput(PRAWINPUT pRawInput) {

	if (joystickMap.find(pRawInput->header.hDevice) != joystickMap.end()) {
		shared_ptr<Joystick> joystick = joystickMap[pRawInput->header.hDevice];
			//wostringstream wss;
			//wss << "Controller: " << joystick->slot << endl;
			//OutputDebugString(wss.str().c_str());
		joystick->parseRawInput(pRawInput);
	}
}

bool Input::matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle) {


	for (HANDLE newHandle : newHandles)
		if (newHandle == joystickHandle)
			return true;
	return false;
}
