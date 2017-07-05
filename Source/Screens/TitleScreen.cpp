#include "../pch.h"
#include "TitleScreen.h"

TitleScreen::~TitleScreen() {
}


#include "../Engine/GameEngine.h"
bool TitleScreen::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) {

	Vector2 dialogPos, dialogSize;
	dialogSize = Vector2(Globals::WINDOW_WIDTH / 3, Globals::WINDOW_HEIGHT / 3);
	dialogPos = Vector2(Globals::targetResolution.x / 2, Globals::targetResolution.y / 2);
	dialogPos.x -= dialogSize.x / 2;
	dialogPos.y -= dialogSize.y / 2;


	noControllerDialog = make_unique<ControllerDialog>(guiFactory.get());
	noControllerDialog->setDimensions(dialogPos, dialogSize);
	noControllerDialog->setLayerDepth(1);
	noControllerDialog->setText(L"Waiting for controller");
	noControllerDialog->setMatrixFunction([&]()->Matrix { return camera->translationMatrix(); });
	noControllerDialog->setCameraZoom([&]()->float { return camera->getZoom(); });


	if (activeSlots.size() == 0)
		noControllerDialog->show();

	pendulum = gfxAssets->getSpriteFromAsset("Pendulum");
	if (!pendulum) {
		GameEngine::errorMessage(L"Could not find pendulum asset");
		return false;
	}
	pendulum->setPosition(Vector2(Globals::WINDOW_WIDTH * .6666, 100));
	pendulum->setOrigin(Vector2(pendulum->getWidth() / 2, 0));
	pendulumRotation = -XM_PIDIV2;
	pendulum->setRotation(pendulumRotation);
	pendulum->setLayerDepth(.9);

	camera->setZoom(1);
	camera->centerOn(Vector2(
		Globals::targetResolution.x / 2, Globals::targetResolution.y / 2));

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		pcSelectDialogs[i] = guiOverlay->createPCSelectDialog(
			guiFactory->getAssetSet("Menu BG 0"), i);

	}
	guiOverlay->initializeTitleScreen(pcSelectDialogs);

	return true;
}


void TitleScreen::setGameManager(GameManager* gm) {
	game = gm;
}


void TitleScreen::reload() {


	pendulum->setPosition(Vector2(Globals::WINDOW_WIDTH * .6666, 100));
	pendulum->setOrigin(Vector2(pendulum->getWidth() / 2, 0));
	pendulumRotation = -XM_PIDIV2;
	pendulum->setRotation(pendulumRotation);

	camera->setZoom(1);
	camera->centerOn(Vector2(
		Globals::targetResolution.x / 2, Globals::targetResolution.y / 2));

	guiOverlay->reloadTitleScreen(pcSelectDialogs);
}

//TOTAL_SWING_TIME = 2 * XM_PI * sqrt(pendulum->getHeight() / GRAVITY);
double totalSwingTime = 0;
double g = 981; // assume 1 pixel == 1 cm, therefore gravity is in 1/100 of a pixel per second squared
float angularAcceleration = 0;
float damping = .99989;
bool doneSwinging = false;
void TitleScreen::update(double deltaTime) {


	if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
		game->confirmExit();
	}

	if (!doneSwinging) {
		totalSwingTime += deltaTime;
		/*if (totalSwingTime >= TOTAL_SWING_TIME)
			totalSwingTime = 0;*/
		double equ = abs(double(pendulum->getHeight()) / -g);
		pendulumRotation = sin(totalSwingTime / sqrt(equ))*damping;

		damping -= .0002;
		if (damping <= .000001) {
			doneSwinging = true;
			pendulumRotation = 0;
		}
	}
	/*angularAcceleration = (-g / pendulum->getHeight()) * pendulumRotation;
	angularVelocity += angularAcceleration;
	angularVelocity *= damping;
	pendulumRotation += angularVelocity;*/

	pendulum->setRotation(pendulumRotation);

	if (noControllerDialog->isOpen()) {
		noControllerDialog->update(deltaTime);
	} else {

		EnterCriticalSection(&cs_activeSlotsAccess);
		bool ready = true;
		int leader = 0;
		for (int i = 0; i < activeSlots.size(); ++i) {
			if (activeSlots[i]->getPlayerSlotNumber() == playerWithMenuControl) {
				leader = i;
				continue;
			} else if (!activeSlots[i]->characterSelect(deltaTime))
				ready = false;
		}

		if (!activeSlots[leader]->characterLocked) {
			if (activeSlots[leader]->characterSelect(deltaTime)) {
				guiOverlay->showMenu();
			}
		} else {
			if (!guiOverlay->menuDialog->isOpen())
				guiOverlay->showMenu();
			guiOverlay->menuDialog->update(deltaTime);
			if (guiOverlay->menuDialog->selectionMade) {

				switch (guiOverlay->menuDialog->getSelected()) {
					case TitleItems::QUIT_GAME:
						game->exit();
						break;
					case TitleItems::NEW_GAME:
						if (ready) {
							guiOverlay->menuDialog->hide();
							for (const auto& slot : activeSlots) {
								slot->pcSelectDialog->hide();
							}
							game->loadLevel(Globals::testLevel);
						} else {
							guiOverlay->menuDialog->selectionMade = false;
						}
						break;
					case TitleItems::CANCEL:
						break;
				}
			}
		}

		LeaveCriticalSection(&cs_activeSlotsAccess);
	}

	//DWORD dwResult;
	//for (DWORD i = 0; i< XUSER_MAX_COUNT; i++) {
	//	XINPUT_STATE state;
	//	ZeroMemory(&state, sizeof(XINPUT_STATE));

	//	// Simply get the state of the controller from XInput.
	//	dwResult = XInputGetState(i, &state);

	//	if (dwResult == ERROR_SUCCESS) {
	//		// Controller is connected 
	//		
	//		if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0) {
	//			int hi = 0;
	//		}
	//	} else {
	//		// Controller is not connected 
	//	}
	//}

}


void TitleScreen::draw(SpriteBatch * batch) {

	pendulum->draw(batch);
	noControllerDialog->draw(batch);
}

void TitleScreen::pause() {
}


void TitleScreen::controllerRemoved(ControllerSocketNumber controllerSocket,
	PlayerSlotNumber slotNumber) {

	slotManager->playerSlots[slotNumber]->resetCharacterSelect();


	EnterCriticalSection(&cs_activeSlotsAccess);

	if (activeSlots.size() == 0) {
		noControllerDialog->show();
		guiOverlay->menuDialog->hide();
		guiOverlay->menuDialog->reset();
	} else if (guiOverlay->menuDialog->currentPlayerIs(
		slotManager->playerSlots[slotNumber].get())) {

		guiOverlay->menuDialog->hide();
		guiOverlay->menuDialog->reset();
		// find next playerslot
		for (PlayerSlotNumber i = PlayerSlotNumber::SLOT_1; i <= PlayerSlotNumber::SLOT_3;
			i = PlayerSlotNumber(i + 1)) {
			if (slotManager->playerSlots[i]->hasJoystick()) {
				guiOverlay->menuDialog->pairPlayerSlot(slotManager->playerSlots[i].get());
				wostringstream woo;
				woo << L"Player " << i;
				guiOverlay->menuDialog->setText(woo.str());
				playerWithMenuControl = i;
				break;
			}
		}

	}

	LeaveCriticalSection(&cs_activeSlotsAccess);

}


void TitleScreen::newController(shared_ptr<Joystick> newStick) {


	if (newStick.get()) {
		if (noControllerDialog->isOpen()) {
			noControllerDialog->hide();
			guiOverlay->menuDialog->pairPlayerSlot(
				slotManager->playerSlots[newStick->playerSlotNumber].get());
			wostringstream woo;
			woo << L"Player " << newStick->playerSlotNumber;
			guiOverlay->menuDialog->setText(woo.str());
			playerWithMenuControl = newStick->playerSlotNumber;
		}
	}
}
