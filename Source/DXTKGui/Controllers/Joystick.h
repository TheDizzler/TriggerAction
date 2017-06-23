#pragma once
#include <Windows.h>
//#include "../BaseGraphics/Sprite.h"
//#include "../StringHelper.h"
#define MAX_BUTTONS		128
#include <hidsdi.h>

class PlayerCharacter;

enum PlayerSlotNumber {
	NONE = -1, SLOT_1, SLOT_2, SLOT_3
};

/* Virtual Sockets for controllers. */
enum ControllerSocketNumber {
	SOCKET_1, SOCKET_2, SOCKET_3
};

enum ControlButtons {
	A = 0, B, X, Y, L, R, SELECT, START
};

/* A joystick class with huge thanks to Alexander Bocken
	https://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input */
class Joystick {
public:

	Joystick(ControllerSocketNumber controllerSocket);
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

	/* The virtual controller socket this joystick is plugged in to. */
	ControllerSocketNumber socket;
	PlayerSlotNumber playerSlotNumber = PlayerSlotNumber::NONE;

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
