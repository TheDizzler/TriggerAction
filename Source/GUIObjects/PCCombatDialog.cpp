#include "PCCombatDialog.h"
#include "../Engine/GameEngine.h"

PCCombatDialog::PCCombatDialog(GUIFactory* factory) : DynamicDialog(factory, NULL) {
	miniPortrait = make_unique<Sprite>();

}

PCCombatDialog::~PCCombatDialog() {
}


void PCCombatDialog::loadPC(CharacterData* pcData) {

	auto pcAssets = pcData->assets;
	pointer->load(guiFactory->getAsset("Cursor Hand 2"));

	miniPortrait->load(pcAssets->getAsset("portrait"));
	miniPortrait->setOrigin(Vector2::Zero);
	miniPortrait->setPosition(portraitPos);
}


void PCCombatDialog::setDimensions(const Vector2& position, const Vector2& size) {
	spriteH = guiFactory->getSpriteFromAsset("H");
	spriteM = guiFactory->getSpriteFromAsset("M");
	spriteP = guiFactory->getSpriteFromAsset("P");
}

bool PCCombatDialog::update(double deltaTime) {
	return DynamicDialog::update(deltaTime);
}

void PCCombatDialog::draw(SpriteBatch* batch) {

	miniPortrait->draw(batch);
	spriteH->draw(batch);
	spriteM->draw(batch);
	spriteP->draw(batch);
}


