#include "PCCombatDialog.h"

PCCombatDialog::PCCombatDialog() {
	miniPortrait = make_unique<Sprite>();

}

#include "../Engine/GameEngine.h"
void PCCombatDialog::loadPC(CharacterData* pcData) {

	auto pcAssets = pcData->assets;
	pointer->load(guiFactory->getAsset("Cursor Hand 2"));

	portrait->load(pcAssets->getAsset("portrait"));
	portrait->setOrigin(Vector2::Zero);
	portrait->setPosition(portraitPos);
}


void PCCombatDialog::setDimensions(const Vector2& position, const Vector2& size) {
	spriteH = guiFactory->getSpriteFromAsset("H");
	spriteM = guiFactory->getSpriteFromAsset("M");
	spriteP = guiFactory->getSpriteFromAsset("P");
}

void PCCombatDialog::draw(SpriteBatch* batch) {

	spriteH->draw(batch);
	spriteM->draw(batch);
	spriteP->draw(batch);
}


