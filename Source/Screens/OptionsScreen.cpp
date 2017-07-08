#include "OptionsScreen.h"
#include "../Engine/GameEngine.h"


OptionsScreen::~OptionsScreen() {
}


bool OptionsScreen::initialize(
	ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) {


	for (PlayerSlotNumber i = PlayerSlotNumber::SLOT_1;
		i <= PlayerSlotNumber::SLOT_3; i = PlayerSlotNumber(i + 1)) {

		if (slotManager->playerSlots[i]->hasJoystick()) {


		}
	}

	//bg.load(gfxAssets->getAsset("Option Screen BG"));
	bg.load(guiFactory->getAsset("Transition BG"));
	bg.setPosition(Vector2::Zero);
	bg.setOrigin(Vector2::Zero);
	bg.setLayerDepth(0);
	//scale bg image to screen
	int horzDif = bg.getWidth() - Globals::WINDOW_WIDTH;
	int vertDif = bg.getHeight() - Globals::WINDOW_HEIGHT;
	if (horzDif > 0 || vertDif > 0) {
		// bg image is bigger in one or more dimensions than screen
		if (horzDif > vertDif) {
			float horzRatio = float(Globals::WINDOW_WIDTH) / bg.getWidth();
			bg.setScale(Vector2(horzRatio, horzRatio));
		} else {
			float vertRatio = float(Globals::WINDOW_HEIGHT) / bg.getHeight();
			bg.setScale(Vector2(vertRatio, vertRatio));
		}
	} else {
		if (horzDif < vertDif) {
			float horzRatio = float(Globals::WINDOW_WIDTH) / bg.getWidth();
			bg.setScale(Vector2(horzRatio, horzRatio));
		} else {
			float vertRatio = float(Globals::WINDOW_HEIGHT) / bg.getHeight();
			bg.setScale(Vector2(vertRatio, vertRatio));
		}
	}


	attackLabel.reset(guiFactory->createTextLabel(Vector2(200, 250), L"Attack:"));

	return true;
}


void OptionsScreen::setGameManager(GameManager* gm) {
	game = gm;
}


void OptionsScreen::update(double deltaTime) {
	if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
		game->confirmExit();
	}
	attackLabel->update(deltaTime);
}

void OptionsScreen::draw(SpriteBatch * batch) {

	bg.draw(batch);
	attackLabel->draw(batch);
}

void OptionsScreen::textureDraw(SpriteBatch* batch) {

	batch->Begin(SpriteSortMode_FrontToBack, NULL,
		NULL, NULL, NULL, NULL, camera->translationMatrix());
	{
		draw(batch);
	}
	batch->End();
}


void OptionsScreen::pause() {
}

void OptionsScreen::controllerRemoved(
	ControllerSocketNumber controllerSlot, PlayerSlotNumber slotNumber) {

}

void OptionsScreen::newController(shared_ptr<Joystick> newStick) {
}
