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


void Input::addJoystick(HANDLE handle) {

	shared_ptr<Joystick> newStick = make_shared<Joystick>(++numSticks);
	joysticks.push_back(newStick);
	joystickMap[handle] = newStick;

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
