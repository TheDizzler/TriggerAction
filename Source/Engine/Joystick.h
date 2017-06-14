#pragma once

#define MAX_BUTTONS		128
#include <hidsdi.h>

class PlayerCharacter;

enum ControlButtons {
	A = 0, B, X, Y, L, R, SELECT, START
};

/* A joystick class with huge thanks to Alexander Bocken
	https://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input */
class Joystick {
public:

	Joystick(size_t controllerSlot);
	~Joystick();

	void registerNewHandle(HANDLE handle);
	HANDLE getHandle();

	BOOL bButtonStates[MAX_BUTTONS];
	BOOL lastButtonStates[MAX_BUTTONS];
	LONG lAxisX = 0;
	LONG lAxisY = 0;
	LONG lAxisZ = 0;
	LONG lAxisRz = 0;
	LONG lHat = 0;
	INT  g_NumberOfButtons;


	void parseRawInput(PRAWINPUT pRawInput);

	size_t socket;
	short playerSlotNumber = -1;

	bool yButtonPushed();
	bool xButtonPushed();
	bool aButtonPushed();
	bool bButtonPushed();
	bool lButtonPushed();
	bool rButtonPushed();
	bool startButtonPushed();
	bool selectButtonPushed();


	ControlButtons startButton = ControlButtons::START;
	ControlButtons selectButton = ControlButtons::SELECT;
	ControlButtons attackButton = ControlButtons::Y;
	ControlButtons jumpButton = ControlButtons::X;
	ControlButtons runButton = ControlButtons::B;
	ControlButtons blockButton = ControlButtons::L;

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
		wss << "Slot " << joystick->socket << " data deleting" << endl;
		OutputDebugString(wss.str().c_str());
	}

	ControllerListener* listener;
	shared_ptr<Joystick> joystick;
	
	bool finishFlag = false;
};