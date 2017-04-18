#pragma once

#define MAX_BUTTONS		128
#include <hidsdi.h>

class PlayerCharacter;

/* A joystick class with huge thanks to Alexander Bocken
	https://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input */
class Joystick {
public:

	/*static enum dpad {
		RIGHT, DOWN, LEFT, UP
	};*/

	Joystick( size_t controllerSlot);
	~Joystick();

	void registerNewHandle(HANDLE handle);
	HANDLE getHandle();

	BOOL bButtonStates[MAX_BUTTONS];
	LONG lAxisX = 0;
	LONG lAxisY = 0;
	LONG lAxisZ = 0;
	LONG lAxisRz = 0;
	LONG lHat = 0;
	INT  g_NumberOfButtons;


	void parseRawInput(PRAWINPUT pRawInput);

	size_t slot;
	PlayerCharacter* pc;
	
private:
	HANDLE handle = NULL;

};