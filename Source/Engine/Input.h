#pragma once

#include <Keyboard.h>

#include "../DXTKGui/Controllers/MouseController.h"
#include "Joystick.h"

extern vector<shared_ptr<Joystick>> joysticks;

class ControllerListener {
public:
	ControllerListener(/*Input* gameEngine*/);
	~ControllerListener();


	void addJoysticks(vector<HANDLE> handles);

	void parseRawInput(PRAWINPUT pRawInput);

	virtual void newController(HANDLE joyHandle) = 0;
	virtual void controllerRemoved() = 0;
	void controllerAccepted(shared_ptr<Joystick> newJoy);

	USHORT numSticks = -1;


	vector<shared_ptr<Joystick>> lostDevices;
	bool matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle);

	map<HANDLE, shared_ptr<Joystick>> joystickMap;
protected:
	bool gameInitialized = false;
		//Input* gameEngine;
};


class Input : public ControllerListener {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);



protected:

	unique_ptr<Keyboard> keys;
	shared_ptr<MouseController> mouse;


};


