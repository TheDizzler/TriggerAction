#pragma once

#include <Keyboard.h>

#include "../DXTKGui/Controllers/MouseController.h"
#include "Joystick.h"

extern shared_ptr<Joystick> joysticks[3];
extern vector<shared_ptr<Joystick>> tempJoysticks;

class ControllerListener {
public:
	ControllerListener();
	virtual ~ControllerListener();


	void addJoysticks(vector<HANDLE> handles);

	void parseRawInput(PRAWINPUT pRawInput);

	virtual void newController(HANDLE joyHandle) = 0;
	virtual void controllerRemoved(size_t controllerSlot) = 0;
	void controllerAccepted(HANDLE handle);


	//vector<shared_ptr<Joystick>> lostDevices;
	bool matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle);

	
protected:
	bool gameInitialized = false;
	map<HANDLE, shared_ptr<Joystick>> joystickMap;
	/** When a new controller is detected, they get placed here until a player "claims" it. */
	map<HANDLE, shared_ptr<Joystick>> unclaimedJoysticks;
	vector<int> availableControllerSlots;
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


