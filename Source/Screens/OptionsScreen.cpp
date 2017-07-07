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
