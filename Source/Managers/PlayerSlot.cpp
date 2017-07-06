#include "../pch.h"
#include "PlayerSlot.h"
#include "../GUIObjects/MenuDialog.h"
#include "../GUIObjects/PCStatusDialog.h"
#include "../Engine/GameEngine.h"


vector<shared_ptr<PlayerSlot>> activeSlots;
deque<shared_ptr<PlayerSlot>> waitingSlots;
CRITICAL_SECTION cs_activeSlotsAccess;

PlayerSlot::PlayerSlot(PlayerSlotNumber slotNum) : slotNumber(slotNum) {


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
	if (!joystick)
		return false;
	if (!characterSelected) {
		//if (joystick->lAxisX < -10) {
		if (joystick->isLeftPressed()) {
			repeatDelayTime += deltaTime;
			if (repeatDelayTime >= REPEAT_DELAY) {
				// select character to left
				characterData = gfxAssets->getPreviousCharacter(&currentCharacterNum);
				pcSelectDialog->loadPC(characterData);
				repeatDelayTime = 0;
			}

		//} else if (joystick->lAxisX > 10) {
		} else if (joystick->isRightPressed()) {
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
	} else if (joystick->aButtonPushed() && _threadJoystickData) {
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
	joystick->setPlayerSlotNumber(slotNumber);

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

	joystick->setPlayerSlotNumber(PlayerSlotNumber::NONE);
	joystick = NULL;
	_threadJoystickData = NULL;
}

JoyData* PlayerSlot::getJoyData() {
	return _threadJoystickData;
}


PlayerSlotNumber PlayerSlot::getPlayerSlotNumber() {
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
	InitializeCriticalSection(&cs_activeSlotsAccess);

	for (int i = 0; i < MAX_PLAYERS; ++i)
		playerSlots[PlayerSlotNumber(PlayerSlotNumber::SLOT_1 + i)]
		= make_shared<PlayerSlot>(PlayerSlotNumber(PlayerSlotNumber::SLOT_1 + i));
}

PlayerSlotManager::~PlayerSlotManager() {
	//waitingSlots.clear();
	activeSlots.clear();
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		playerSlots[i].reset();
	}

	DeleteCriticalSection(&cs_waitingJoysticks);
	DeleteCriticalSection(&cs_activeSlotsAccess);
}

bool PlayerSlotManager::checkXInputSlotNumber(USHORT inputSlotNum) {
	for (const auto& pad : gamepads)
		if (pad->getXInputSlot() == inputSlotNum)
			return true;
	return false;
}

void PlayerSlotManager::addGamePad(shared_ptr<GamePadJoystick> newPad) {
	gamepads.push_back(newPad);
}

void PlayerSlotManager::updateGamePads() {
	for (const auto& pad : gamepads) {
		pad->update();
	}
}

void PlayerSlotManager::waiting() {

	accessWaitingSlots(CHECK_FOR_CONFIRM, NULL);

}


void PlayerSlotManager::gamePadRemoved(shared_ptr<Joystick> gamePad) {

	wstringstream wss;
	wss << "GamePad in PlayerSlot " << gamePad->getPlayerSlotNumber() << " removed" << endl;
	OutputDebugString(wss.str().c_str());

	JoyData* joydata = playerSlots[gamePad->getPlayerSlotNumber()]->getJoyData();
	if (joydata)
		accessWaitingSlots(REMOVE_FROM_LIST, joydata);

	PlayerSlotNumber slotNum = gamePad->getPlayerSlotNumber();
	accessActiveSlots(REMOVE_FROM_LIST, (PVOID) &slotNum);
	playerSlots[gamePad->getPlayerSlotNumber()]->unpairSocket();

	gamepads.erase(remove(gamepads.begin(), gamepads.end(),
		gamePad), gamepads.end());

}


void PlayerSlotManager::controllerRemoved(shared_ptr<Joystick> joystick) {

	wstringstream wss;
	wss << "Controller PlayerSlot " << joystick->getPlayerSlotNumber() << " removed" << endl;
	OutputDebugString(wss.str().c_str());

	JoyData* joydata = playerSlots[joystick->getPlayerSlotNumber()]->getJoyData();
	if (joydata)
		accessWaitingSlots(REMOVE_FROM_LIST, joydata);

	PlayerSlotNumber slotNum = joystick->getPlayerSlotNumber();
	accessActiveSlots(REMOVE_FROM_LIST, (PVOID) &slotNum);
	playerSlots[joystick->getPlayerSlotNumber()]->unpairSocket();
}


void PlayerSlotManager::controllerTryingToPair(JoyData* joyData) {

	accessWaitingSlots(ADD_TO_LIST, joyData);

}


void PlayerSlotManager::finalizePair(JoyData* joyData) {

	accessWaitingSlots(REMOVE_FROM_LIST, joyData);

	PlayerSlotNumber plyrSltNum = joyData->joystick->getPlayerSlotNumber();
	accessActiveSlots(ADD_TO_LIST, &plyrSltNum);
	//activeSlots.push_back(playerSlots[joyData->joystick->playerSlotNumber]);
	/*if (joyData->isXInput)
		gamepads.push_back((GamePadJoystick*) joyData->joystick.get());*/

	playerSlots[joyData->joystick->getPlayerSlotNumber()]->selectCharacter();
	playerSlots[joyData->joystick->getPlayerSlotNumber()]->finishInit();

	wostringstream ws;
	ws << L"Player ";
	ws << (playerSlots[joyData->joystick->getPlayerSlotNumber()]->getPlayerSlotNumber() + 1);
	playerSlots[joyData->joystick->getPlayerSlotNumber()]->setDialogText(ws.str());
}


void PlayerSlotManager::accessWaitingSlots(size_t task, PVOID pvoid) {

	int* playerSlotNumber;
	JoyData* joyData = (JoyData*) pvoid;


	EnterCriticalSection(&cs_waitingJoysticks);
	//OutputDebugString(L"\nEntering CS -> ");
	switch (task) {
		case ADD_TO_LIST:
			for (int i = 0; i < MAX_PLAYERS; ++i) {
				if (playerSlots[i]->pairWithSocket(joyData)) {
					waitingSlots.push_back(playerSlots[i]);
					DWORD id;
					CreateThread(NULL, 0, slotManagerThread, (PVOID) 0, 0, &id);
					break;
				}
			}
			break;

		case REMOVE_FROM_LIST:
			waitingSlots.erase(remove(waitingSlots.begin(), waitingSlots.end(),
				playerSlots[joyData->joystick->getPlayerSlotNumber()]));
			break;

		case CHECK_FOR_CONFIRM:
			for (const auto& slot : waitingSlots)
				slot->waiting();
			break;

	}
	//OutputDebugString(L"Exiting CS\n");
	LeaveCriticalSection(&cs_waitingJoysticks);
}


void PlayerSlotManager::accessActiveSlots(size_t task, PVOID pvoid) {

	PlayerSlotNumber playerSlotNumber = *((PlayerSlotNumber*) pvoid);

	EnterCriticalSection(&cs_activeSlotsAccess);
	OutputDebugString(L"\nEntering CS -> ");
	switch (task) {
		case ADD_TO_LIST:
			activeSlots.push_back(playerSlots[playerSlotNumber]);
			break;
		case REMOVE_FROM_LIST:
			activeSlots.erase(remove(activeSlots.begin(), activeSlots.end(),
				playerSlots[playerSlotNumber]), activeSlots.end());
			break;

	}
	OutputDebugString(L"Exiting CS\n");
	LeaveCriticalSection(&cs_activeSlotsAccess);
}