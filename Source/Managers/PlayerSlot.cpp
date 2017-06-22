#include "../pch.h"
#include "PlayerSlot.h"
#include "../GUIObjects/MenuDialog.h"
#include "../GUIObjects/PCStatusDialog.h"
#include "../Engine/GameEngine.h"
//#include "../Engine/Input.h"

vector<shared_ptr<PlayerSlot>> activeSlots;
deque<shared_ptr<PlayerSlot>> waitingSlots;


PlayerSlot::PlayerSlot(size_t slotNum) : slotNumber(slotNum) {


}


PlayerSlot::~PlayerSlot() {
	pauseDialog.reset();
}


void PlayerSlot::resetCharacterSelect() {

	characterLocked = false;
	characterSelected = false;
	pcSelectDialog->reset();
	if (gfxAssets->setCharacterSelected(currentCharacterNum, false))
	//characterData = gfxAssets->getNextCharacter(&currentCharacterNum);
		pcSelectDialog->loadPC(characterData);
}


bool PlayerSlot::characterSelect(double deltaTime) {

	//if (!characterLocked) {

	if (!characterSelected) {
		if (joystick->lAxisX < -10) {
			repeatDelayTime += deltaTime;
			if (repeatDelayTime >= REPEAT_DELAY) {
				// select character to left
				characterData = gfxAssets->getPreviousCharacter(&currentCharacterNum);
				pcSelectDialog->loadPC(characterData);
				repeatDelayTime = 0;
			}

		} else if (joystick->lAxisX > 10) {
			repeatDelayTime += deltaTime;
			if (repeatDelayTime >= REPEAT_DELAY) {
				// select character to right
				characterData = gfxAssets->getNextCharacter(&currentCharacterNum);
				pcSelectDialog->loadPC(characterData);
				repeatDelayTime = 0;
			}
		} else {
			repeatDelayTime = REPEAT_DELAY;
		}
	}



	if (joystick->aButtonPushed()) {


		if (characterSelected
			&& gfxAssets->setCharacterSelected(currentCharacterNum, true)) {
			characterLocked = true;

			pcSelectDialog->setReady(true);
		} else {
			characterSelected = true;
			pcSelectDialog->setSelected(true);
		}

	} else if (joystick->bButtonPushed()) {


		if (characterLocked || characterSelected) {
			if (characterLocked)
				gfxAssets->setCharacterSelected(currentCharacterNum, false);
			characterLocked = false;
			characterSelected = false;
			pcSelectDialog->setSelected(false);
			pcSelectDialog->setReady(false);

		}
	}
//}
	return characterLocked;
}


void PlayerSlot::waiting() {

	if (joystick == NULL) {
		GameEngine::errorMessage(L"Joystick non-existant");
	} else if (joystick->aButtonPushed()) {
		_threadJoystickData->finishFlag = true;
		// after this the waiting thread will execute ControllerListener->playerAcceptedSlot(joyData)
		_threadJoystickData = NULL;
	}
}

void PlayerSlot::finishInit() {
	if (_threadJoystickData)
		_threadJoystickData->finishFlag = true;
	_threadJoystickData = NULL;
}


void PlayerSlot::pairWithDialog(PCSelectDialog* dialog) {
	pcSelectDialog = dialog;
}

void PlayerSlot::pairWithStatusDialog(PCStatusDialog* dialog) {
	statusDialog = dialog;
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

	pcSelectDialog->show();
	pcSelectDialog->setText(L"Push A\nto join!");
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

	pcSelectDialog->hide();

	joystick->playerSlotNumber = -1;
	joystick = NULL;
	_threadJoystickData = NULL;
}

JoyData* PlayerSlot::getJoyData() {
	return _threadJoystickData;
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

	currentCharacterNum += slotNumber;
	characterData = gfxAssets->getNextCharacter(&currentCharacterNum);
	pcSelectDialog->loadPC(characterData);
}

void PlayerSlot::setDialogText(wstring text) {
	pcSelectDialog->setText(text);
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


void PlayerSlotManager::controllerRemoved(shared_ptr<Joystick> joystick) {

	wstringstream wss;
	wss << "Controller PlayerSlot " << joystick->playerSlotNumber << " removed" << endl;
	OutputDebugString(wss.str().c_str());
	JoyData* joydata = playerSlots[joystick->playerSlotNumber]->getJoyData();

	if (joydata /*&& joydata->joystick*/)
		accessWaitingSlots(REMOVE_FROM_LIST, joydata);

	activeSlots.erase(remove(activeSlots.begin(), activeSlots.end(),
		playerSlots[joystick->playerSlotNumber]), activeSlots.end());
	playerSlots[joystick->playerSlotNumber]->unpairSocket();
}


void PlayerSlotManager::controllerTryingToPair(JoyData* joyData) {

	accessWaitingSlots(ADD_TO_WAITING_LIST, joyData);

}


void PlayerSlotManager::finalizePair(JoyData* joyData) {

	accessWaitingSlots(REMOVE_FROM_LIST, joyData);

	activeSlots.push_back(playerSlots[joyData->joystick->playerSlotNumber]);

	playerSlots[joyData->joystick->playerSlotNumber]->selectCharacter();
	playerSlots[joyData->joystick->playerSlotNumber]->finishInit();

	wostringstream ws;
	ws << L"Player ";
	ws << (playerSlots[joyData->joystick->playerSlotNumber]->getPlayerSlotNumber() + 1);
	playerSlots[joyData->joystick->playerSlotNumber]->setDialogText(ws.str());
}


void PlayerSlotManager::accessWaitingSlots(size_t task, PVOID pvoid) {

	int* playerSlotNumber;
	JoyData* joyData = (JoyData*) pvoid;
	//if (joyData == NULL)
	//	//return;
	//	playerSlotNumber = (int*) pvoid;

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

