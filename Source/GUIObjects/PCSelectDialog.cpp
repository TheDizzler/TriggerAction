#include "../pch.h"
#include "PCSelectDialog.h"

#include "../Engine/GameEngine.h"
PCSelectDialog::PCSelectDialog() {

	portrait = make_unique<Sprite>();
	magicEnglish = make_unique<Sprite>();

	pointer = make_unique<SelectionPointer>();
}


void PCSelectDialog::reset() {

	playerReady = false;
}

void PCSelectDialog::loadPC(shared_ptr<AssetSet> pcAssets) {

	pointer->load(guiFactory->getAsset("Cursor Hand 2"));

	portrait->load(pcAssets->getAsset("portrait"));
	portrait->setOrigin(Vector2::Zero);
	portrait->setPosition(portraitPos);

	magicEnglish->load(pcAssets->getAsset("magic english"));
	magicEnglish->setOrigin(Vector2::Zero);
	magicEnglish->setPosition(magicPos);

	playerReady = true;
}


const int PORTRAIT_WIDTH = 48;
void PCSelectDialog::setDimensions(const Vector2& position, const Vector2 & size) {

	DynamicDialog::setDimensions(position, size);

	portraitPos = dialogText->getPosition();
	portraitPos.y += dialogText->measureString(L"Test").y + dialogTextMargin.y;

	magicPos = portraitPos;
	magicPos.x += PORTRAIT_WIDTH + dialogTextMargin.x;

	pointerPos = portraitPos;
	pointerPos.y += PORTRAIT_WIDTH / 2;
	pointer->setPosition(pointerPos);
}



#include "../Screens/GUIOverlay.h"
void PCSelectDialog::update(double deltaTime) {
	//DynamicDialog::update(deltaTime);

	if (!isShowing)
		return;

	if (!playerReady) {
		if (playerSlot->joystick->bButtonStates[0]) {
			_threadJoystickData->finishFlag = true;

			//playerReady = true;
		}
	}
	pointer->update(deltaTime);

}

void PCSelectDialog::draw(SpriteBatch* batch) {

	if (!isShowing)
		return;

	texturePanel->draw(batch);
	dialogText->draw(batch);

	if (playerReady) {
		portrait->draw(batch);
		magicEnglish->draw(batch);
		pointer->draw(batch);
	}
}


void PCSelectDialog::textureDraw(SpriteBatch* batch) {
	DynamicDialog::textureDraw(batch);
	//portrait->draw(batch);
}

void PCSelectDialog::hide() {
	DynamicDialog::hide();
	reset();
}
