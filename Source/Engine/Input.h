#pragma once
//#define SLOT_OF_DEATH 69

#include <Keyboard.h>


#include "../DXTKGui/Controllers/MouseController.h"
#include "../Managers/PlayerSlot.h"

extern unique_ptr<PlayerSlotManager> slotManager;



DWORD WINAPI waitForHUDThread(PVOID pVoid);
DWORD WINAPI waitForPlayerThread(PVOID pVoid);



class ControllerListener {
public:
	ControllerListener();
	virtual ~ControllerListener();


	void addJoysticks(vector<HANDLE> handles);

	void parseRawInput(PRAWINPUT pRawInput);

	virtual void newController(shared_ptr<Joystick> newStick) = 0;
	virtual void controllerRemoved(size_t controllerSlot) = 0;
	//void pairSocketToPlayerSlot(JoyData* joyData);
	void unpairedJoystickRemoved(JoyData* joyData);
	void playerAcceptedSlot(JoyData* joyData);

	bool matchFound(vector<HANDLE> newHandles, HANDLE joystickHandle);

	/** This is a virtual representation of physical controller ports. */
	shared_ptr<Joystick> joystickPorts[MAX_PLAYERS];
protected:
	
	map<HANDLE, shared_ptr<Joystick>> joystickMap;
	/** When a new controller is detected, they get placed here until a player "claims" it. */
	//map<HANDLE, shared_ptr<Joystick>> unclaimedJoysticks;
	deque<USHORT> availableControllerSockets;

	/* Thread Safe. */
	bool socketsAvailable();
	/** Thread safe. Does not check if any slots available.*/
	USHORT getNextAvailableControllerSocket();

	enum SharedResourceTask {
		CHECK_SOCKETS_AVAILBLE, GET_NEXT_AVAILABLE
	};
	USHORT sharedResource(size_t task);

	CRITICAL_SECTION cs_availableControllerSockets;

	
};


class Input : public ControllerListener {
public:
	Input();
	virtual ~Input();

	bool initRawInput(HWND hwnd);


	shared_ptr<MouseController> mouse;
protected:

	unique_ptr<Keyboard> keys;
	


};
