#include "../pch.h"
#include "PCSelectDialog.h"

#include "../Engine/GameEngine.h"
PCSelectDialog::PCSelectDialog() {
}


void PCSelectDialog::loadPC(shared_ptr<AssetSet> pcAssets) {

	portrait = make_unique<Sprite>();
	portrait->load(pcAssets->getAsset("portrait"));
	portrait->setOrigin(Vector2::Zero);
	
	magicEnglish = make_unique<Sprite>();
	magicEnglish->load(pcAssets->getAsset("magic english"));
	magicEnglish->setOrigin(Vector2::Zero);

}



void PCSelectDialog::setDimensions(const Vector2& position, const Vector2 & size) {

	DynamicDialog::setDimensions(position, size);
	Vector2 pos = dialogText->getPosition();
	pos.y +=  dialogText->measureString(L"Test").y + dialogTextMargin.y;
	portrait->setPosition(pos);
	pos.x += portrait->getWidth() + dialogTextMargin.x;
	magicEnglish->setPosition(pos);
}




void PCSelectDialog::draw(SpriteBatch* batch) {

	if (!isShowing)
		return;

	texturePanel->draw(batch);
	dialogText->draw(batch);
	portrait->draw(batch);
	magicEnglish->draw(batch);
}


void PCSelectDialog::textureDraw(SpriteBatch* batch) {
	DynamicDialog::textureDraw(batch);
	//portrait->draw(batch);
}
