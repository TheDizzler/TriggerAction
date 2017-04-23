#pragma once

#include "../globals.h"
#include "../Engine/Joystick.h"





class PCSelectDialog;

/** This class is how the game communicates with the player input.
	The windows message pump sends data to the joysticks and the game queries
	the playerslots for needed data. */
class PlayerSlot {
public:
	PlayerSlot(size_t slotNum) : slotNumber(slotNum) {
	}

	void pairWithDialog(PCSelectDialog* dialog);

	/** Returns true if pair was succesful. Returns false if slot already paired. */
	bool pairWithSocket(JoyData* joyData);
	void unpairSocket();

	//void loadCharacterData(const CharacterData* characterData);


	size_t getPlayerSlotNumber();
	bool hasJoystick();
	Joystick* getStick();


	void setDialogText(wstring text);
	PCSelectDialog* pcDialog;
private:
	Joystick* joystick = NULL;
	size_t slotNumber;

	

	/* For temporary initialization purposes only! Do not use! */
	JoyData* _threadJoystickData;
};

extern shared_ptr<PlayerSlot>  playerSlots[MAX_PLAYERS];

class PlayerSlotManager {
public:
	PlayerSlotManager();
	~PlayerSlotManager();
	
	void controllerRemoved(size_t playerSlot);
	void controllerTryingToPair(JoyData* joyData);
	void finalizePair(JoyData* joyData);



private:
	CRITICAL_SECTION cs_waitingJoysticks;
	//vector<JoyData*> waitingForInput;
	
	/*enum WaitingForInputTast {
	READ_INPUT, REMOVE_JOYSTICK_FROM_WAITING
	};
	void sharedResource(size_t task, PVOID pvoid);*/
};