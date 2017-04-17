#include "../pch.h"
#include "TitleScreen.h"

TitleScreen::TitleScreen(vector<shared_ptr<Joystick>> joys) {
	//joysticks = joys;
}

TitleScreen::~TitleScreen() {
	//joysticks->clear();
}

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
	if (joysticks.size() <= 0)
		noControllerDialog->show();

	pendulum = gfxAssets->getSpriteFromAsset("Pendulum");
	if (!pendulum) {
		GameEngine::errorMessage(L"Could not find pendulum asset");
		return false;
	}
	pendulum->setPosition(Vector2(Globals::WINDOW_WIDTH * .6666, -10));
	pendulum->setOrigin(Vector2(pendulum->getWidth() / 2, 0));
	pendulum->setLayerDepth(.9);
	return true;
}

void TitleScreen::setGameManager(GameManager* gm) {
	game = gm;
}

//double dialogOpenTime = 0;
//const double CONTROLLER_WAIT_TIME = 1.0;
//int ellipsisii = 16;
void TitleScreen::update(double deltaTime, shared_ptr<MouseController> mouse) {

	if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
		game->confirmExit();
	}

	/*if (joysticks.size() <= 0) {
		noControllerDialog->show();
		dialogOpenTime += deltaTime;
		if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
			dialogOpenTime = 0;
			if (ellipsisii++ > 4) {
				ellipsisii = 0;
				noControllerDialog->setText(L"Waiting for controller");
			} else {
				wstring text = noControllerDialog->getText();
				text += L".\n";
				noControllerDialog->setText(text);
			}
		}
		noControllerDialog->update(deltaTime);
	} else {
		noControllerDialog->close();
	}*/
	if (noControllerDialog->isOpen)
		noControllerDialog->update(deltaTime);
	else
		quitButton->update(deltaTime);
}


void TitleScreen::draw(SpriteBatch * batch) {

	quitButton->draw(batch);
	pendulum->draw(batch);

	noControllerDialog->draw(batch);
}

void TitleScreen::pause() {
}


void TitleScreen::controllerRemoved(size_t controllerSlot) {

	if (joysticks.size() <= 0) {
		noControllerDialog->show();
	}
}


void TitleScreen::newController(HANDLE joyHandle) {

	shared_ptr<Joystick> newStick = make_shared<Joystick>(joyHandle, joysticks.size() + 1);
	game->controllerAccepted(newStick);

	if (joysticks.size() > 0) {
		noControllerDialog->close();
	}
}
