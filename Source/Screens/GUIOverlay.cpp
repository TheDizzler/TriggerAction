#include "../pch.h"
#include "GUIOverlay.h"

const int TEXT_MARGIN = 5;
const int TEST_BOX_MARGIN = 16;
#include "../Engine/GameEngine.h"
GUIOverlay::GUIOverlay() {




	unique_ptr<TextLabel> textLabel;
	textLabel.reset(guiFactory->createTextLabel(Vector2(-100, -100)));




	Vector2 textMeasure = textLabel->measureString(L"TECH");
	// should be enough to fit 4 lines of text + margin
	int windowHeight = textMeasure.y * 4 + TEXT_MARGIN;
	Vector2 pos, size;
	size = Vector2(Globals::WINDOW_WIDTH * 2 / 3, windowHeight);
	pos = Vector2(Globals::WINDOW_WIDTH - size.x, TEST_BOX_MARGIN);
	hudDialogs[HUDDIALOG::PLAYERSTATS] = createPCDialog(guiFactory->getAssetSet("Menu BG 0"), pos, size);
	hudDialogs[HUDDIALOG::ENEMIES] = createPCDialog(guiFactory->getAssetSet("Menu BG 0"), pos, size);
	//hudDialogs[HUDDIALOG::PLAYERSTATS]->setDimensions(pos, size);
	pos = Vector2(TEST_BOX_MARGIN * 2, TEST_BOX_MARGIN);
	//hudDialogs[HUDDIALOG::ENEMIES]->setDimensions(pos, size);

	// should be wide enough to fit four letter word and the hand icon
	size.x = textMeasure.x * 2 + guiFactory->getAsset("Cursor Hand 1")->getWidth() + TEXT_MARGIN;

	pos.y = TEST_BOX_MARGIN;
	hudDialogs[HUDDIALOG::PLAYER1]
		= createPCDialog(guiFactory->getAssetSet("Menu BG 0"), pos, size);
	pos.x += hudDialogs[HUDDIALOG::PLAYER1]->getWidth();

	hudDialogs[HUDDIALOG::PLAYER2]
		= createPCDialog(guiFactory->getAssetSet("Menu BG 0"), pos, size);

	pos.x += hudDialogs[HUDDIALOG::PLAYER2]->getWidth();

	hudDialogs[HUDDIALOG::PLAYER3]
		= createPCDialog(guiFactory->getAssetSet("Menu BG 0"), pos, size);


	for (int i = 0; i < MAX_PLAYERS; ++i) {
		playerSlots[i] = make_shared<PlayerSlot>(i);
		playerSlots[i]->pairWithDialog(hudDialogs[HUDDIALOG::PLAYER1 + i].get());
		
	}

	fpsLabel.reset(guiFactory->createTextLabel(Vector2(Globals::WINDOW_WIDTH - 250, 20)));
	fpsLabel->setTint(Colors::Black);
	fpsLabel->setScale(Vector2(.5, .5));
	fpsLabel->setLayerDepth(1);


	InitializeCriticalSection(&cs_waitingJoysticks);
}

GUIOverlay::~GUIOverlay() {

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		//playerSlots[i]->unpairSocket();
		playerSlots[i].reset();
	}
	//delete playerSlots;
	lostJoyDialogs.clear();
	for (auto& dialog : hudDialogs)
		dialog.reset();

	DeleteCriticalSection(&cs_waitingJoysticks);

}

int frameCount = 0;
double fpsUpdateTime = 5;
const double FPS_UPDATE_TIME = 1;
void GUIOverlay::update(double deltaTime, shared_ptr<MouseController> mouse) {

	fpsUpdateTime += deltaTime;
	++frameCount;
	if (fpsUpdateTime >= FPS_UPDATE_TIME) {

		wostringstream wss;
		wss << "frameCount: " << frameCount << " fpsUpdateTime: " << fpsUpdateTime << endl;
		wss << "fps: " << frameCount / fpsUpdateTime;
		fpsLabel->setText(wss);

		fpsUpdateTime = 0;
		frameCount = 0;
	}


	/*for (const auto& slot : playerSlots) {
		if (slot->joystick->bButtonStates[0]) {

		}
	}*/

	/*if (waitingForInput.size() > 0) {
		sharedResource(READ_INPUT, NULL);
	}*/
	for (const auto& dialog : lostJoyDialogs) {
		dialog->update(deltaTime);
	}

	for (const auto& dialog : hudDialogs)
		dialog->update(deltaTime);
}

void GUIOverlay::draw(SpriteBatch* batch) {

	for (const auto& dialog : hudDialogs)
		dialog->draw(batch);

	for (const auto& dialog : lostJoyDialogs)
		dialog->draw(batch);


	fpsLabel->draw(batch);
}

void GUIOverlay::setDialogText(USHORT playerSlot, wstring text) {

	hudDialogs[PLAYER1 + playerSlot]->setText(text);
	hudDialogs[PLAYER1 + playerSlot]->show();
}

#include "../DXTKGui/StringHelper.h"
void GUIOverlay::reportLostJoystick(size_t joystickSocket) {

	displayingLostJoys.push_back(joystickSocket);
	shared_ptr<Joystick> lostJoy = joystickSlots[joystickSocket];

	unique_ptr<ControllerDialog> joyLostDialog;
	size_t numDialogs = lostJoyDialogs.size();
	Vector2 dialogPos, dialogSize;
	if (numDialogs <= 0)
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 4, Globals::WINDOW_HEIGHT / 4);
	else
		dialogSize = Vector2(lostJoyDialogs[0]->getWidth(), lostJoyDialogs[0]->getHeight());


	dialogPos = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	dialogPos.y -= dialogSize.y / 2;

	if (numDialogs <= 0) {
		dialogPos.x -= dialogSize.x / 2;
	} else {
		for (const auto& dialog : lostJoyDialogs)
			dialog->moveBy(Vector2(-dialogSize.x / 2, 0));
		dialogPos.x += (dialogSize.x / 2) * (numDialogs - 1);
	}


	joyLostDialog = make_unique<ControllerDialog>(guiFactory.get());
	joyLostDialog->setDimensions(dialogPos, dialogSize);
	wostringstream title;
	title << L"Player " << lostJoy->socket;
	title << L"  has dropped." << endl;
	joyLostDialog->setTitle(title.str(), Vector2(1.2, 1.2));
	wostringstream wss;
	//wss << StringHelper::convertCharStarToWCharT(lostJoy->pc->name.c_str()) << endl;
	wss << L"Waiting for controller...\n";
	joyLostDialog->setText(wss.str());
	joyLostDialog->show();

	lostJoyDialogs.push_back(move(joyLostDialog));
}


void GUIOverlay::controllerRemoved(size_t playerSlot) {

	wstringstream wss;
	wss << "Controller is PlayerSlot " << playerSlot << " removed" << endl;
	OutputDebugString(wss.str().c_str());

	playerSlots[playerSlot]->pcDialog->hide();
	playerSlots[playerSlot]->unpairSocket();
}


void GUIOverlay::controllerTryingToPair(JoyData* joyData) {

	EnterCriticalSection(&cs_waitingJoysticks);
	OutputDebugString(L"\nEntering CS -> ");
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		if (playerSlots[i]->pairWithSocket(joyData->joystick)) {
			playerSlots[i]->pcDialog->show();
			playerSlots[i]->pcDialog->setText(L"Push A\nto join!");
			playerSlots[i]->pcDialog->_threadJoystickData = joyData;
			/*hudDialogs[PLAYER1 + i]->show();
			hudDialogs[PLAYER1 + i]->setText(L"Push A\nto join!");*/
			break;
		}
	}
	OutputDebugString(L"Exiting CS\n");
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


void GUIOverlay::finalizePair(JoyData* joyData) {

	readyPCSelect(playerSlots[joyData->joystick->playerSlot]);
}


void GUIOverlay::readyPCSelect(shared_ptr<PlayerSlot> playerSlot) {

	wostringstream ws;
	ws << L"Player " << (playerSlot->slotNumber + 1);
	playerSlot->pcDialog->setText(ws.str());

	EnterCriticalSection(&cs_waitingJoysticks);
	{
		playerSlot->pcDialog->loadPC(gfxAssets->getAssetSet(pcs[nextAvaiablePC++]));
		if (nextAvaiablePC > numPCsAvailable - 1)
			nextAvaiablePC = 0;
	}
	LeaveCriticalSection(&cs_waitingJoysticks);
}


unique_ptr<PCSelectDialog> GUIOverlay::createPCDialog(shared_ptr<AssetSet> dialogImageSet,
	const Vector2& position, const Vector2& size, const char_t* fontName) {

	unique_ptr<PCSelectDialog> dialog = make_unique<PCSelectDialog>();
	dialog->initializeControl(guiFactory.get(), NULL);
	dialog->initialize(dialogImageSet, fontName);
	dialog->setDimensions(position, size);
	dialog->texturize();
	return move(dialog);
}




ControllerDialog::ControllerDialog(GUIFactory* guiF)
	: PromptDialog(guiF->getHWND(), false, true) {

	initializeControl(guiF, guiF->getMouseController());
	initialize(guiFactory->getAsset("White Pixel"));

}


void ControllerDialog::setDimensions(const Vector2& position, const Vector2& size,
	const int frameThickness) {

	PromptDialog::setDimensions(position, size, frameThickness);
}


void ControllerDialog::update(double deltaTime) {

	if (!isShowing)
		return;


	dialogOpenTime += deltaTime;
	if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
		dialogOpenTime = 0;
		if (ellipsisii++ > 5) {
			ellipsisii = 0;
			setText(defaultText);
		} else {
			wstring text = dialogText->getText();
			text += L".";
			PromptDialog::setText(text);
		}
	}
	PromptDialog::update(deltaTime);
}

void ControllerDialog::setText(wstring text) {

	defaultText = text;
	PromptDialog::setText(text);
}
