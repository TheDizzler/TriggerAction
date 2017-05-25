#include "../pch.h"
#include "PlayerSlot.h"

vector<shared_ptr<PlayerSlot>> activeSlots;
deque<shared_ptr<PlayerSlot>> waitingSlots;

#include "../GUIObjects/MenuDialog.h"
#include "../Engine/GameEngine.h"
PlayerSlot::PlayerSlot(size_t slotNum) : slotNumber(slotNum) {


}


PlayerSlot::~PlayerSlot() {
	pauseDialog.reset();
}


bool PlayerSlot::characterSelect(double deltaTime) {

	PCSelectDialog* dialog = (PCSelectDialog*) pcDialog;
	if (characterLocked) {

	} else {
		
		if (!characterSelected) {
			if (joystick->lAxisX < -10) {
				repeatDelayTime += deltaTime;
				if (repeatDelayTime >= REPEAT_DELAY) {
					// select character to left
					characterData = gfxAssets->getPreviousCharacter(&currentCharacterNum);
					dialog->loadPC(characterData);
					repeatDelayTime = 0;
				}

			} else if (joystick->lAxisX > 10) {
				repeatDelayTime += deltaTime;
				if (repeatDelayTime >= REPEAT_DELAY) {
					// select character to right
					characterData = gfxAssets->getNextCharacter(&currentCharacterNum);
					dialog->loadPC(characterData);
					repeatDelayTime = 0;
				}
			} else {
				repeatDelayTime = REPEAT_DELAY;
			}
		}
	}


	if (joystick->bButtonStates[ControlButtons::A]) {
		if (!buttonStillDown) {
			buttonStillDown = true;

			if (characterSelected) {
				characterLocked = true;
				dialog->setReady(true);
			} else {
				characterSelected = true;
				dialog->setSelected(true);
			}
		}
	} else if (joystick->bButtonStates[ControlButtons::B]) {
		if (!buttonStillDown) {
			buttonStillDown = true;

			if (characterLocked || characterSelected) {
				characterLocked = false;
				characterSelected = false;
				dialog->setSelected(false);
				dialog->setReady(false);
			}
		}
	} else {
		buttonStillDown = false;
	}

	return characterLocked;
}


void PlayerSlot::waiting() {

	if (joystick->bButtonStates[ControlButtons::A]) {
		_threadJoystickData->finishFlag = true;
		// after this the waiting thread will execute ControllerListener->playerAcceptedSlot(joyData)

		// "lock" A button until released
		buttonStillDown = true;
	}
}


void PlayerSlot::pairWithDialog(DynamicDialog* dialog) {
	pcDialog = dialog;
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

	pauseDialog = make_unique<MenuDialog>(guiFactory.get());
	pauseDialog->initialize(guiFactory->getAssetSet("Menu BG 1"));
	pauseDialog->setDimensions(
		Vector2(Globals::WINDOW_WIDTH / 3, Globals::WINDOW_HEIGHT / 2),
		Vector2(100, 100));

	pauseDialog->pairPlayerSlot(this);
	wostringstream woo;
	woo << L"Player Slot " << slotNumber;
	pauseDialog->setText(woo.str());
	pauseDialog->clearSelections();
	pauseDialog->addSelection(L"Continue", true);
	pauseDialog->addSelection(L"Settings", false);
	pauseDialog->addSelection(L"Quit", true);
	pauseDialog->addSelection(L"Reload XML", true);
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

void PlayerSlot::selectCharacter() {

	characterData = gfxAssets->getNextCharacter(&currentCharacterNum);
	((PCSelectDialog*) pcDialog)->loadPC(characterData);
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
	waitingSlots.clear();
	activeSlots.clear();
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		playerSlots[i].reset();
	}
}

void PlayerSlotManager::waiting() {

	if (waitingSlots.size() > 0)
		accessWaitingSlots(CHECK_FOR_CONFIRM, NULL);

}


void PlayerSlotManager::controllerRemoved(size_t playerSlotNumber) {

	wstringstream wss;
	wss << "Controller PlayerSlot " << playerSlotNumber << " removed" << endl;
	OutputDebugString(wss.str().c_str());

	activeSlots.erase(remove(activeSlots.begin(), activeSlots.end(),
		playerSlots[playerSlotNumber]), activeSlots.end());
	playerSlots[playerSlotNumber]->unpairSocket();
}


void PlayerSlotManager::controllerTryingToPair(JoyData* joyData) {

	accessWaitingSlots(ADD_TO_WAITING_LIST, joyData);

}


void PlayerSlotManager::finalizePair(JoyData* joyData) {

	accessWaitingSlots(REMOVE_FROM_LIST, joyData);

	activeSlots.push_back(playerSlots[joyData->joystick->playerSlotNumber]);

	playerSlots[joyData->joystick->playerSlotNumber]->selectCharacter();

	wostringstream ws;
	ws << L"Player ";
	ws << (playerSlots[joyData->joystick->playerSlotNumber]->getPlayerSlotNumber() + 1);
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

