#include "../pch.h"
#include "PlayerSlot.h"

vector<shared_ptr<PlayerSlot>> activeSlots;
deque<shared_ptr<PlayerSlot>> waitingSlots;

//#include "../Screens/GUIOverlay.h"
#include "../Engine/GameEngine.h"
void PlayerSlot::characterSelect(double deltaTime) {


	if (joystick->lAxisX < -10) {
		repeatDelayTime += deltaTime;
		if (repeatDelayTime >= REPEAT_DELAY) {
			// select character to left
			characterData = gfxAssets->getPreviousCharacter(&currentCharacterNum);
			pcDialog->loadPC(characterData->assets);
			repeatDelayTime = 0;
		}

	} else if (joystick->lAxisX > 10) {
		repeatDelayTime += deltaTime;
		if (repeatDelayTime >= REPEAT_DELAY) {
			// select character to right
			characterData = gfxAssets->getNextCharacter(&currentCharacterNum);
			pcDialog->loadPC(characterData->assets);
			repeatDelayTime = 0;
		}
	} else {
		repeatDelayTime = REPEAT_DELAY;
	}
}


void PlayerSlot::waiting() {

	if (joystick->bButtonStates[0]) {
		_threadJoystickData->finishFlag = true;

		//wostringstream ws;
		//ws << L"Player " << (getPlayerSlotNumber() + 1);
		//setDialogText(ws.str());
	}
}


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
	joystick->playerSlotNumber = slotNumber;

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

	joystick->playerSlotNumber = -1;
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

//#include "../DXTKGui/StringHelper.h"
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

void PlayerSlotManager::waiting() {

	if (activeSlots.size() > 0)
		accessWaitingSlots(CHECK_FOR_CONFIRM, NULL);

}


void PlayerSlotManager::controllerRemoved(size_t playerSlotNumber) {

	wstringstream wss;
	wss << "Controller is PlayerSlot " << playerSlotNumber << " removed" << endl;
	OutputDebugString(wss.str().c_str());

	playerSlots[playerSlotNumber]->unpairSocket();
}


void PlayerSlotManager::controllerTryingToPair(JoyData* joyData) {

	accessWaitingSlots(ADD_TO_WAITING_LIST, joyData);

	/*EnterCriticalSection(&cs_waitingJoysticks);
	OutputDebugString(L"\nEntering CS -> ");
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		if (playerSlots[i]->pairWithSocket(joyData)) {
			waitingSlots.push_back(playerSlots[i]);
			break;
		}
	}
	OutputDebugString(L"Exiting CS\n");
	LeaveCriticalSection(&cs_waitingJoysticks);*/
}


void PlayerSlotManager::finalizePair(JoyData* joyData) {

	accessWaitingSlots(REMOVE_FROM_LIST, joyData);
	//waitingSlots.erase(remove(waitingSlots.begin(), waitingSlots.end(), playerSlots[joyData->joystick->playerSlotNumber]));

	activeSlots.push_back(playerSlots[joyData->joystick->playerSlotNumber]);
	//guiOverlay->readyPCSelect(playerSlots[joyData->joystick->playerSlotNumber]);

	wostringstream ws;
	ws << L"Player " << (playerSlots[joyData->joystick->playerSlotNumber]->getPlayerSlotNumber() + 1);
	playerSlots[joyData->joystick->playerSlotNumber]->setDialogText(ws.str());
}


void PlayerSlotManager::accessWaitingSlots(size_t task, PVOID pvoid) {

	JoyData* joyData = (JoyData*) pvoid;

	EnterCriticalSection(&cs_waitingJoysticks);
	//OutputDebugString(L"\nEntering CS -> ");
	switch (task) {
		case ADD_TO_WAITING_LIST:
			for (int i = 0; i < MAX_PLAYERS; ++i) {
				if (playerSlots[i]->pairWithSocket(joyData)) {
					waitingSlots.push_back(playerSlots[i]);
					break;
				}
			}
			break;

		case REMOVE_FROM_LIST:
			waitingSlots.erase(remove(waitingSlots.begin(), waitingSlots.end(),
				playerSlots[joyData->joystick->playerSlotNumber]));
			break;

		case CHECK_FOR_CONFIRM:
			for (const auto& slot : waitingSlots)
				slot->waiting();
			break;

	}
	//OutputDebugString(L"Exiting CS\n");
	LeaveCriticalSection(&cs_waitingJoysticks);
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

