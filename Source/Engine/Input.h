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
	

	vector<shared_ptr<Joystick>> deviceLost;
	
protected:

	unique_ptr<KeyboardController> keys;
	shared_ptr<MouseController> mouse;
	
	USHORT numSticks = -1;
	map<HANDLE, shared_ptr<Joystick>> joystickMap;
	vector<shared_ptr<Joystick>> joysticks;

	//map<tagRAWINPUTDEVICELIST*, shared_ptr<Joystick>> joystickDeviceMap;

	bool matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle);
};

