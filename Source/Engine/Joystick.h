#pragma once

#define MAX_BUTTONS		24
#include <hidsdi.h>

class PlayerCharacter;

class Joystick {
public:

	/*static enum dpad {
		RIGHT, DOWN, LEFT, UP
	};*/

	Joystick(HANDLE handle, size_t controllerSlot);
	~Joystick();

	void registerNewHandle(HANDLE handle);


	BOOL bButtonStates[MAX_BUTTONS];
	LONG lAxisX;
	LONG lAxisY;
	LONG lAxisZ;
	LONG lAxisRz;
	LONG lHat;
	INT  g_NumberOfButtons;


	void parseRawInput(PRAWINPUT pRawInput);

	size_t slot;
	PlayerCharacter* pc;
	HANDLE handle;
private:


};