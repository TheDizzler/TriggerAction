#include "../pch.h"
#include "PCSelectDialog.h"

#include "../Engine/GameEngine.h"
#include "../Screens/GUIOverlay.h"

PCSelectDialog::PCSelectDialog(GUIFactory* guiF) : DynamicDialog(guiF, NULL) {

	portrait = make_unique<Sprite>();
	magicEnglish = make_unique<Sprite>();
	weaponType = make_unique<Sprite>();
	pointer = make_unique<SelectionPointer>();
	pointer->load(guiFactory->getAsset("Cursor Hand 2"));

}

PCSelectDialog::~PCSelectDialog() {
}


void PCSelectDialog::reset() {

	showReadyLabel = false;
	playerReady = false;
	pointer->reset();
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

	weaponTypePos = magicPos;
	weaponTypePos.y += PORTRAIT_WIDTH / 2;

	readyLabelPos = portraitPos;
	readyLabelPos.y += PORTRAIT_WIDTH / 2;
	readyLabel.reset(guiFactory->createTextLabel(Vector2::Zero));
	readyLabel->setPosition(readyLabelPos);
	readyLabel->setRotation(-XM_PIDIV4 * .5);
	readyLabel->setTint(Colors::Red);
	readyLabel->setScale(Vector2(1.75, 1.75));
	readyLabel->setText(L"Ready!");
}


void PCSelectDialog::loadPC(CharacterData* pcData) {

	auto pcAssets = pcData->assets;

	portrait->load(pcAssets->getAsset("portrait"));
	portrait->setOrigin(Vector2::Zero);
	portrait->setPosition(portraitPos);

	magicEnglish->load(pcAssets->getAsset("magic english"));
	magicEnglish->setOrigin(Vector2::Zero);
	magicEnglish->setPosition(magicPos);

	weaponType->load(guiFactory->getAssetSet("Menu Icons")->getAsset(pcData->weaponType.c_str()));
	weaponType->setOrigin(Vector2::Zero);
	weaponType->setPosition(weaponTypePos);

	playerReady = true;
}



bool PCSelectDialog::update(double deltaTime) {

	if (!isShowing)
		return false;

	bool refreshed = true;

	if (DynamicDialog::update(deltaTime))
		refreshed = true;
	pointer->update(deltaTime);

	return refreshed;
}

void PCSelectDialog::draw(SpriteBatch* batch) {

	if (!isShowing)
		return;

	DynamicDialog::draw(batch);

	if (playerReady) {
		portrait->draw(batch);
		magicEnglish->draw(batch);
		weaponType->draw(batch);
		if (showReadyLabel)
			readyLabel->draw(batch);
		else
			pointer->draw(batch);
	}
}


void PCSelectDialog::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {
	DynamicDialog::textureDraw(batch);
	
	/*portrait->draw(batch);
	magicEnglish->draw(batch);
	weaponType->draw(batch);*/


}

void PCSelectDialog::hide() {
	DynamicDialog::hide();
}

void PCSelectDialog::show() {
	DynamicDialog::show();
	reset();
}

void PCSelectDialog::setSelected(bool selected) {
	pointer->setSelected(selected);
}

void PCSelectDialog::setReady(bool isReady) {
	showReadyLabel = isReady;
}
