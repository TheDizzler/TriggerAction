#pragma once

#define MAX_BUTTONS		24
#include <hidsdi.h>

class Joystick {
public:

	static enum dpad {
		RIGHT, DOWN, LEFT, UP
	};


	Joystick(USHORT controllerSlot);
	~Joystick();

	BOOL bButtonStates[MAX_BUTTONS];
	LONG lAxisX;
	LONG lAxisY;
	LONG lAxisZ;
	LONG lAxisRz;
	LONG lHat;
	INT  g_NumberOfButtons;


	void parseRawInput(PRAWINPUT pRawInput);

	USHORT slot = -1;
private:


};