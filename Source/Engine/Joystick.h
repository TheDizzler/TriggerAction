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


class ControllerListener;
/** This class is used for passing awaiting joysticks around threads. */
struct JoyData {

	JoyData(shared_ptr<Joystick> joy, ControllerListener* conListener)
		: joystick(joy), listener(conListener) {
	}
	~JoyData() {
		wostringstream wss;
		wss << "Slot " << joystick->slot << " data deleting" << endl;
		OutputDebugString(wss.str().c_str());
	}

	void playerAcceptedSlot();
	void removeFromUnclaimed();

	ControllerListener* listener;
	shared_ptr<Joystick> joystick;
	int tempSlot;
	bool finishFlag = false;
};