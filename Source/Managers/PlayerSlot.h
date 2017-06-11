#pragma once

#include "../globals.h"
#include "../Engine/Joystick.h"
#include "../GameObjects/Characters/CharacterData.h"
#include <deque>

class MenuDialog;
class PCStatusDialog;

enum ControlButtons {
	A = 0, B, X, Y, L, R, SELECT, START
};

const double REPEAT_DELAY = .5;

class PCSelectDialog;

/** This class is how the game communicates with the player input.
	The windows message pump sends data to the joysticks and the game queries
	the playerslots for needed data. */
class PlayerSlot {
public:
	PlayerSlot(size_t slotNum);
	virtual ~PlayerSlot();

	bool characterSelect(double deltaTime);
	void waiting();


	void pairWithDialog(DynamicDialog* dialog);
	void pairWithStatusDialog(PCStatusDialog* dialog);

	/** Returns true if pair was succesful. Returns false if slot already paired. */
	bool pairWithSocket(JoyData* joyData);
	void unpairSocket();



	size_t getPlayerSlotNumber();
	bool hasJoystick();
	Joystick* getStick();

	void selectCharacter();

	void setDialogText(wstring text);
	DynamicDialog* pcDialog;
	PCStatusDialog* statusDialog;
	CharacterData* characterData;

	unique_ptr<MenuDialog> pauseDialog;
private:
	Joystick* joystick = NULL;
	size_t slotNumber;

	double repeatDelayTime = REPEAT_DELAY;

	int currentCharacterNum = -1;
	bool characterSelected = false;
	bool characterLocked = false;
	//bool buttonStillDown = false;

	/* For temporary initialization purposes only! Do not use! */
	JoyData* _threadJoystickData;
};

extern vector<shared_ptr<PlayerSlot>> activeSlots;
extern deque<shared_ptr<PlayerSlot>> waitingSlots;

class PlayerSlotManager {
public:
	PlayerSlotManager();
	~PlayerSlotManager();

	void waiting();


	void controllerRemoved(size_t playerSlotNumber);
	void controllerTryingToPair(JoyData* joyData);
	void finalizePair(JoyData* joyData);

	shared_ptr<PlayerSlot>  playerSlots[MAX_PLAYERS];

private:
	CRITICAL_SECTION cs_waitingJoysticks;

	enum WaitingForInputTast {
		ADD_TO_WAITING_LIST, REMOVE_FROM_LIST, CHECK_FOR_CONFIRM
	};
	void accessWaitingSlots(size_t task, PVOID pvoid);
};