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

	keys.reset(new KeyboardController());
	mouse = make_shared<MouseController>(hwnd);
	if (guiFactory != NULL)
		mouse->loadMouseIcon(guiFactory.get(), "Mouse Icon");

	return true;
}


//void Input::addJoystick(HANDLE handle) {
//
//
//	if (joystickMap[handle]) {
//		OutputDebugString(L"That joystick already registered.\n");
//	} else {
//		shared_ptr<Joystick> newStick = make_shared<Joystick>(handle, ++numSticks);
//		joystickMap[handle] = newStick;
//		joysticks.push_back(newStick);
//		OutputDebugString(L"New joystick found!\n");
//	}
//
//}

void Input::addJoysticks(vector<HANDLE> handles) {

	if (handles.size() < joystickMap.size()) {
		// joystick was removed it - find it!
		for (const auto& joyDev : joystickMap) {
			if (matchFound(handles, joyDev.first))
				continue;
			//OutputDebugString(L"Found removed joystick\n");
			deviceLost.push_back(joyDev.second);

		}
	} else if (handles.size() > joystickMap.size()) {
		for (const auto& newHandle : handles) {
			if (joystickMap[newHandle]) {
				OutputDebugString(L"That joystick already registered.\n");
			} else if (deviceLost.size() > 0) {
				// create joystick and wait for 
			
			
			}else {
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

	shared_ptr<Joystick> joystick = joystickMap[pRawInput->header.hDevice];
	if (joystick != NULL) {
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
