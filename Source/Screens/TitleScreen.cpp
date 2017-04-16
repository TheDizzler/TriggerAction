#include "../pch.h"
#include "TitleScreen.h"

TitleScreen::TitleScreen(vector<shared_ptr<Joystick>> joys) {
	joysticks = joys;
}

TitleScreen::~TitleScreen() {
	joysticks.clear();
}

#include "../Engine/GameEngine.h"
bool TitleScreen::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) {

	quitButton.reset(guiFactory->createButton(Vector2(200, 200), Vector2(10, 10), L"Quit"));
	quitButton->setOnClickListener(new OnClickListenerDialogQuitButton(game));
	//quitButton->setLayerDepth(0.0);

	Vector2 dialogPos, dialogSize;
	dialogSize = Vector2(Globals::WINDOW_WIDTH / 3, Globals::WINDOW_HEIGHT / 3);
	dialogPos = dialogSize;
	dialogPos.x -= dialogSize.x / 2;
	dialogPos.y -= dialogSize.y / 2;
	noControllerDialog = guiFactory->createDialog(dialogPos, dialogSize, 2, true, true);
	//noControllerDialog->setTint(Color(1, .5, 1, 1));
	noControllerDialog->setTitle(L"Test");
	noControllerDialog->setLayerDepth(1);

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

double dialogOpenTime = 0;
const double CONTROLLER_WAIT_TIME = 1.0;
int ellipsisii = 4;
void TitleScreen::update(double deltaTime, shared_ptr<MouseController> mouse) {

	if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
		game->confirmExit();
	}

	if (joysticks.size() <= 0) {
		noControllerDialog->open();
		dialogOpenTime += deltaTime;
		if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
			dialogOpenTime = 0;
			if (ellipsisii++ > 3) {
				ellipsisii = 0;
				noControllerDialog->setText(L"Waiting for controller\n");
			} else {
				wstring text = noControllerDialog->getText();
				text += L".";
				noControllerDialog->setText(text);
			}
		}
		noControllerDialog->update(deltaTime);
	} else {
		noControllerDialog->close();
		ellipsisii = 4;
		dialogOpenTime = 0;
	}
	quitButton->update(deltaTime);
}

//void TitleScreen::updatePaused(double deltaTime) {
//
//	//displayPause(deltaTime);
//
//	Color color = pauseLabel->getTint();
//	/** Changes the Red variable between 0 and 1. */
//	if (rInc) {
//		color.R(color.R() + deltaTime);
//		if (color.R() >= 1)
//			rInc = false;
//	} else {
//		color.R(color.R() - deltaTime);
//		if (color.R() <= 0)
//			rInc = true;
//	}
//
//	pauseLabel->setTint(color);
//
//
//}

void TitleScreen::draw(SpriteBatch * batch) {

	quitButton->draw(batch);
	pendulum->draw(batch);

	noControllerDialog->draw(batch);
}

void TitleScreen::pause() {
}


void TitleScreen::controllerRemoved() {
}
