#include "../pch.h"
#include "PCStatusDialog.h"

PCStatusDialog::PCStatusDialog(GUIFactory* guiF) : DynamicDialog(guiF, NULL) {

	/*portrait = make_unique<Sprite>();
	magicEnglish = make_unique<Sprite>();
	weaponType = make_unique<Sprite>();
	pointer = make_unique<SelectionPointer>();
	pointer->load(guiFactory->getAsset("Cursor Hand 2"));*/


}

PCStatusDialog::~PCStatusDialog() {
}



void PCStatusDialog::setDimensions(const Vector2& pos, const Vector2& sz) {
	DynamicDialog::setDimensions(pos, sz);

	Vector2 size = dialogText->getFont()->measureString(L"Tech");
	dialogText->getFont();
	namePosition = pos + dialogTextMargin;
	hpPosition = namePosition;
	hpPosition.y += size.y + dialogTextMargin.y;

	mpPosition = hpPosition;
	mpPosition.y += size.y + dialogTextMargin.y;
}


void PCStatusDialog::loadPC(PlayerCharacter* pc) {

	player = pc;
	{
		wostringstream woo;
		woo << player->name.c_str();
		nameLabel.reset(
			guiFactory->createTextLabel(
				namePosition, woo.str(), dialogText->getFont()->fontName));
	}
	{
		wostringstream woo;
		woo << "HP: " << player->currentHP << "/" << player->maxHP;
		hpLabel.reset(
			guiFactory->createTextLabel(
				hpPosition, woo.str(), dialogText->getFont()->fontName));
	}
	{
		wostringstream woo;
		woo << "MP: " << player->currentMP << "/" << player->maxMP;
		mpLabel.reset(
			guiFactory->createTextLabel(
				mpPosition, woo.str(), dialogText->getFont()->fontName));
	}
}

void PCStatusDialog::update(double deltaTime) {

	if (!isShowing)
		return;

	DynamicDialog::update(deltaTime);
	nameLabel->update(deltaTime);
	hpLabel->update(deltaTime);
	mpLabel->update(deltaTime);
}

void PCStatusDialog::draw(SpriteBatch* batch) {

	if (!isShowing)
		return;

	DynamicDialog::draw(batch);

	nameLabel->draw(batch);
	hpLabel->draw(batch);
	mpLabel->draw(batch);
}

void PCStatusDialog::updateHP() {
	wostringstream woo;
	woo << "HP: " << player->currentHP << "/" << player->maxHP;
	hpLabel->setText(woo.str());
}

void PCStatusDialog::updateMP() {
	wostringstream woo;
	woo << "MP: " << player->currentMP << "/" << player->maxMP;
	mpLabel->setText(woo.str());
}
