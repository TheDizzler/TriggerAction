#include "../pch.h"
#include "PlayerSlot.h"

shared_ptr<PlayerSlot>  playerSlots[MAX_PLAYERS];


#include "../Screens/GUIOverlay.h"
void PlayerSlot::pairWithDialog(PCSelectDialog* dialog) {
	pcDialog = dialog;
	pcDialog->pairPlayerSlot(this);
}


bool PlayerSlot::pairWithSocket(JoyData* joyData) {
	if (joystick != NULL) {
		wostringstream wss;
		wss << L"Player Slot " << slotNumber << " already connected" << endl;
		OutputDebugString(wss.str().c_str());
		return false;
	}
	joystick = joyData->joystick.get();
	joystick->playerSlot = slotNumber;

	pcDialog->show();
	pcDialog->setText(L"Push A\nto join!");
	_threadJoystickData = joyData;

	wostringstream wss;
	wss << L"Connecting Socket " << joystick->socket << L" to Player Slot " << slotNumber << endl;
	OutputDebugString(wss.str().c_str());
	return true;
}


void PlayerSlot::unpairSocket() {
	wostringstream wss;
	wss << L"Socket " << joystick->socket << L" and Player Slot " << slotNumber;
	wss << L" unpaired." << endl;
	OutputDebugString(wss.str().c_str());

	pcDialog->hide();

	joystick->playerSlot = -1;
	joystick = NULL;

}


size_t PlayerSlot::getPlayerSlotNumber() {
	return slotNumber;
}

bool PlayerSlot::hasJoystick() {
	return joystick != NULL;
}

Joystick* PlayerSlot::getStick() {
	return joystick;
}

void PlayerSlot::setDialogText(wstring text) {
	pcDialog->setText(text);
}




/** ******** PLAYERSLOT MANAGER START ******** **/

#include "../DXTKGui/StringHelper.h"
PlayerSlotManager::PlayerSlotManager() {
	InitializeCriticalSection(&cs_waitingJoysticks);

	for (int i = 0; i < MAX_PLAYERS; ++i)
		playerSlots[i] = make_shared<PlayerSlot>(i);
}

PlayerSlotManager::~PlayerSlotManager() {
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		playerSlots[i].reset();
	}
}



void PlayerSlotManager::controllerRemoved(size_t playerSlot) {

	wstringstream wss;
	wss << "Controller is PlayerSlot " << playerSlot << " removed" << endl;
	OutputDebugString(wss.str().c_str());

	playerSlots[playerSlot]->unpairSocket();
}


void PlayerSlotManager::controllerTryingToPair(JoyData* joyData) {

	EnterCriticalSection(&cs_waitingJoysticks);
	OutputDebugString(L"\nEntering CS -> ");
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		if (playerSlots[i]->pairWithSocket(joyData)) {
			break;
		}
	}
	OutputDebugString(L"Exiting CS\n");
	LeaveCriticalSection(&cs_waitingJoysticks);
}

#include "../Engine/GameEngine.h"
void PlayerSlotManager::finalizePair(JoyData* joyData) {

	guiOverlay->readyPCSelect(playerSlots[joyData->joystick->playerSlot]);
}

//void GUIOverlay::unpairedJoystickRemoved(JoyData* joyData) {
//
//	//sharedResource(REMOVE_JOYSTICK_FROM_WAITING, joyData);
//
//	controllerRemoved(joyData->joystick->socket);
//}



//void GUIOverlay::sharedResource(size_t task, PVOID pvoid) {
//
//	EnterCriticalSection(&cs_waitingJoysticks);
//	switch (task) {
//		case READ_INPUT:
//			for (const auto& joy : waitingForInput) {
//				if (joy->joystick->bButtonStates[0]) {
//					joy->finishFlag = true;
//				}
//			}
//			break;
//
//		case REMOVE_JOYSTICK_FROM_WAITING:
//			JoyData* joyData = (JoyData*) pvoid;
//			for (int i = 0; i < waitingForInput.size(); ++i) {
//				if (waitingForInput[i] == joyData) {
//					swap(waitingForInput[i], waitingForInput.back());
//					waitingForInput.pop_back();
//				}
//			}
//
//			if (!joyData->pairingKilled)
//				guiOverlay->readyPCSelect(joyData->joystick->socket);
//			break;
//	}
//	LeaveCriticalSection(&cs_waitingJoysticks);
//}



//void GUIOverlay::pairPlayerSlotWith(JoyData* joyData) {
//
//	sharedResource(REMOVE_JOYSTICK_FROM_WAITING, joyData);
//
//}

