#pragma once


#include "../DXTKGui/Controllers/MouseController.h"
#include "../DXTKGui/Controllers/KeyboardController.h"
#include "Joystick.h"


class Input {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);

	//void addJoystick(HANDLE handle);
	void addJoysticks(vector<HANDLE> handles);
	void controllerRemoved(PDEV_BROADCAST_DEVICEINTERFACE removedDevice);

	void parseRawInput(PRAWINPUT pRawInput);


	

protected:

	unique_ptr<KeyboardController> keys;
	shared_ptr<MouseController> mouse;

	USHORT numSticks = -1;

	vector<shared_ptr<Joystick>> joysticks;
	vector<shared_ptr<Joystick>> lostDevices;
	bool matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle);

private:
	map<HANDLE, shared_ptr<Joystick>> joystickMap;
};

