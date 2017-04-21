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
	quitButton->setOnClickListener(new OnClickListenerDialogQuitButton(game));
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

	Vector2 pos = Vector2::Zero;
	for (int i = 0; i < 300; ++i) {
		

	}
	camera->centerOn(Vector2(256 / 2, 224 / 2));

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
void TitleScreen::update(double deltaTime, shared_ptr<MouseController> mouse) {

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

		//for (int i = 0; i < tempJoysticks.size(); ++i) {
		//	if (tempJoysticks[i]->bButtonStates[0]) {
		//		game->controllerAccepted(tempJoysticks[i]->getHandle());
		//		swap(tempJoysticks[i], tempJoysticks.back());
		//		tempJoysticks.pop_back();
		//		break;
		//	}
		//}

		//for (const auto& joy : joysticks) {
		//	if (joy->getHandle()) {
		//		noControllerDialog->close();
		//		break;
		//	}
		//}
	} else
		quitButton->update(deltaTime);
}


void TitleScreen::draw(SpriteBatch * batch) {

	quitButton->draw(batch);
	pendulum->draw(batch);

	noControllerDialog->draw(batch);

	/*for (const auto& frame : frames)
		frame->draw(batch);*/
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

	noControllerDialog->hide();

}
