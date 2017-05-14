#include "../pch.h"
#include "TitleScreen.h"

//TitleScreen::TitleScreen() {
//}

TitleScreen::~TitleScreen() {

}

const float GRAVITY = 981; // assume 1 pixel == 1 cm, therefore gravity is in 1/100 of a pixel per second squared
float TOTAL_SWING_TIME;

#include "../Engine/GameEngine.h"
bool TitleScreen::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) {

	quitButton.reset(guiFactory->createButton(Vector2(200, 200), Vector2(10, 10), L"Quit"));
	quitButton->setActionListener(new OnClickListenerDialogQuitButton(game));
	//quitButton->setLayerDepth(0.0);


	Vector2 dialogPos, dialogSize;
	dialogSize = Vector2(Globals::WINDOW_WIDTH / 3, Globals::WINDOW_HEIGHT / 3);
	dialogPos = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	dialogPos.x -= dialogSize.x / 2;
	dialogPos.y -= dialogSize.y / 2;


	noControllerDialog = make_unique<ControllerDialog>(guiFactory.get());
	noControllerDialog->setDimensions(dialogPos, dialogSize);
	noControllerDialog->setLayerDepth(1);
	noControllerDialog->setText(L"Waiting for controller");



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


	camera->centerOn(Vector2(256 / 2, 224 / 2));

	guiOverlay->initializeTitleScreen();

	return true;
}

void TitleScreen::setGameManager(GameManager* gm) {
	game = gm;
}

//TOTAL_SWING_TIME = 2 * XM_PI * sqrt(pendulum->getHeight() / GRAVITY);
double totalSwingTime = 0;
double g = GRAVITY;
float angularAcceleration = 0;
float damping = .99989;
bool doneSwinging = false;
bool openedOnce = false;
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
		//slotManager->waiting();
	} else {

		bool ready = true;
		for (const auto& slot : activeSlots) {
			if (!slot->characterSelect(deltaTime))
				ready = false;
		}

		if (!openedOnce && ready) {
			guiOverlay->showMenu();
			openedOnce = true;
		}
		guiOverlay->menuDialog->update(deltaTime);
		if (guiOverlay->menuDialog->selectionMade) {

			switch (guiOverlay->menuDialog->getSelected()) {
				case TitleItems::QUIT_GAME:
					game->exit();
					break;
				case TitleItems::NEW_GAME:
					//state = CHARACTER_SELECT;
					guiOverlay->menuDialog->hide();
					game->loadLevel(Globals::testLevel);
					break;

			}
		}
	}
	quitButton->update(deltaTime);


	slotManager->waiting();

}


void TitleScreen::draw(SpriteBatch * batch) {

	quitButton->draw(batch);
	pendulum->draw(batch);

	noControllerDialog->draw(batch);
}

void TitleScreen::pause() {
}


void TitleScreen::controllerRemoved(size_t controllerSlot) {

	/*for (const auto& slot : playerSlots) {
		if (slot->hasJoystick()) {
			return;
		}
	}
	noControllerDialog->show();*/

	if (activeSlots.size() == 0)
		noControllerDialog->show();
}


void TitleScreen::newController(HANDLE joyHandle) {

	if (joyHandle)
		noControllerDialog->hide();

}
