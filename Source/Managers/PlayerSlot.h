#pragma once

#include "../globals.h"
#include "../DXTKGui/Controllers/Joystick.h"
#include "../GameObjects/Characters/CharacterData.h"
#include <deque>



extern CRITICAL_SECTION cs_activeSlotsAccess;

const double REPEAT_DELAY = .5;

class MenuDialog;
class PCStatusDialog;
class PCSelectDialog;
class JoyData;

/** This class is how the game communicates with the player input.
	The windows message pump sends data to the joysticks and the game queries
	the playerslots for needed data. */
class PlayerSlot {
public:
	PlayerSlot(PlayerSlotNumber slotNum);
	virtual ~PlayerSlot();

	void resetCharacterSelect();
	bool characterSelect(double deltaTime);
	void waiting();
	void finishInit();

	void pairWithDialog(PCSelectDialog* dialog);
	void pairWithStatusDialog(PCStatusDialog* dialog);

	/** Returns true if pair was succesful. Returns false if slot already paired. */
	bool pairWithSocket(JoyData* joyData);
	void unpairSocket();

	JoyData* getJoyData();

	PlayerSlotNumber getPlayerSlotNumber();
	/** If no joy, no player :( */
	bool hasJoystick();
	Joystick* getStick();

	void selectCharacter();

	void setDialogText(wstring text);
	PCSelectDialog* pcSelectDialog;
	PCStatusDialog* statusDialog;
	CharacterData* characterData;
	bool characterLocked = false;

	unique_ptr<MenuDialog> pauseDialog;
private:
	Joystick* joystick = NULL;
	PlayerSlotNumber slotNumber;

	double repeatDelayTime = REPEAT_DELAY;

	int currentCharacterNum = -1;
	bool characterSelected = false;

	/* For temporary initialization purposes only! Do not use! */
	JoyData* _threadJoystickData;
};

extern vector<shared_ptr<PlayerSlot>> activeSlots;
extern deque<shared_ptr<PlayerSlot>> waitingSlots;

class PlayerSlotManager {
public:
	PlayerSlotManager();
	~PlayerSlotManager();

	/* GamePads are added as soon as they are discovered. */
	void addGamePad(shared_ptr<GamePadJoystick> newPad);
	void updateGamePads();

	void waiting();

	void controllerRemoved(shared_ptr<Joystick> joystick);
	void controllerTryingToPair(JoyData* joyData);
	void finalizePair(JoyData* joyData);

	/** Ordered by PlayerSlotNumber. Always 3, but not all 3 are always in use. */
	shared_ptr<PlayerSlot>  playerSlots[MAX_PLAYERS];

private:
	CRITICAL_SECTION cs_waitingJoysticks;

	enum WaitingForInputTast {
		ADD_TO_LIST, REMOVE_FROM_LIST, CHECK_FOR_CONFIRM
	};
	void accessWaitingSlots(size_t task, PVOID pvoid);
	void accessActiveSlots(size_t task, PVOID pvoid);

	vector<shared_ptr<GamePadJoystick>> gamepads;
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

	//bool isXInput = false;
	bool finishFlag = false;
};