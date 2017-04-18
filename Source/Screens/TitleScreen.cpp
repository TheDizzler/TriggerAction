#include "../pch.h"
#include "TitleScreen.h"

//TitleScreen::TitleScreen() {
//}

TitleScreen::~TitleScreen() {

}

//float TOTAL_SWING_TIME;
const float GRAVITY = .0000981; // assume 1 pixel == 1 cm, therefore gravity is in 1/100 of a pixel per second squared

#include "../Engine/GameEngine.h"
bool TitleScreen::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) {

	quitButton.reset(guiFactory->createButton(Vector2(200, 200), Vector2(10, 10), L"Quit"));
	quitButton->setOnClickListener(new OnClickListenerDialogQuitButton(game));
	//quitButton->setLayerDepth(0.0);

	Vector2 dialogPos, dialogSize;
	dialogSize = Vector2(Globals::WINDOW_WIDTH / 3, Globals::WINDOW_HEIGHT / 3);
	dialogPos = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	dialogPos.x -= dialogSize.x / 2;
	dialogPos.y -= dialogSize.y / 2;
	//noControllerDialog = guiFactory->createDialog(dialogPos, dialogSize, true, true);
	//noControllerDialog->setTint(Color(1, .5, 1, 1));
	noControllerDialog = make_unique<ControllerDialog>(guiFactory.get());
	noControllerDialog->setDimensions(dialogPos, dialogSize);
	noControllerDialog->setLayerDepth(1);
	noControllerDialog->setText(L"Waiting for controller");

	bool noJoys = true;
	for (const auto& joy : joysticks) {
		if (joy->getHandle()) {
			noJoys = false;
			break;
		}
	}
	if (noJoys)
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

	//TOTAL_SWING_TIME = 2 * XM_PI * sqrt(pendulum->getHeight() / GRAVITY);


	testDialog = guiFactory->createDialog(guiFactory->getAssetSet("Menu BG 0"), Vector2(100, 100));


	camera->centerOn(Vector2(256 / 2, 224 / 2));

	return true;
}

void TitleScreen::setGameManager(GameManager* gm) {
	game = gm;
}

int direction = -1;
float angularAcceleration = 0;
float damping = .99989;
void TitleScreen::update(double deltaTime, shared_ptr<MouseController> mouse) {

	if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
		game->confirmExit();
	}

	//float g = GRAVITY;

	angularAcceleration = (-GRAVITY / pendulum->getHeight()) * pendulumRotation;
	angularVelocity += angularAcceleration;
	angularVelocity *= damping;
	pendulumRotation += angularVelocity;

	//if (joysticks[0]->bButtonStates[0])
		pendulum->setRotation(pendulumRotation);

	if (noControllerDialog->isOpen) {
		noControllerDialog->update(deltaTime);

		for (int i = 0; i < tempJoysticks.size(); ++i) {
			if (tempJoysticks[i]->bButtonStates[0]) {
				game->controllerAccepted(tempJoysticks[i]->getHandle());
				swap(tempJoysticks[i], tempJoysticks.back());
				tempJoysticks.pop_back();
				break;
			}
		}

		for (const auto& joy : joysticks) {
			if (joy->getHandle()) {
				noControllerDialog->close();
				break;
			}
		}
	} else
		quitButton->update(deltaTime);
}


void TitleScreen::draw(SpriteBatch * batch) {

	quitButton->draw(batch);
	pendulum->draw(batch);

	noControllerDialog->draw(batch);

	testDialog->draw(batch);
}

void TitleScreen::pause() {
}


void TitleScreen::controllerRemoved(size_t controllerSlot) {

	bool noJoys = true;
	for (const auto& joy : joysticks) {
		if (joy->getHandle()) {
			noJoys = false;
			break;
		}
	}
	if (noJoys)
		noControllerDialog->show();
}


void TitleScreen::newController(HANDLE joyHandle) {

	//for (const auto& joy : joysticks) {
	//	if (joy->getHandle()) {
	//		noControllerDialog->close();
	//		break;
	//	}
	//}

}
