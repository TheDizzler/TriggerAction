#pragma once


#include "../DXTKGui/Controllers/MouseController.h"
#include "../DXTKGui/Controllers/KeyboardController.h"
#include "Joystick.h"


class Input {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);

	void addJoystick(HANDLE handle);
	void parseRawInput(PRAWINPUT pRawInput);
	

	
protected:

	unique_ptr<KeyboardController> keys;
	shared_ptr<MouseController> mouse;
	
	USHORT numSticks = -1;
	map<HANDLE, shared_ptr<Joystick>> joystickMap;
	vector<shared_ptr<Joystick>> joysticks;

	//shared_ptr<GamePad> gamePad;

};

