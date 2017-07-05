#include "../pch.h"
#include "MenuDialog.h"


MenuDialog::MenuDialog(GUIFactory* guiF) : DynamicDialog(guiF, NULL) {

	pointer = make_unique<SelectionPointer>();
	pointer->load(guiFactory->getAsset("Cursor Hand 2"));
}

MenuDialog::~MenuDialog() {
}

void MenuDialog::pairPlayerSlot(PlayerSlot* slot) {
	playerSlot = slot;
}

void MenuDialog::reset() {
	//playerSlot = NULL;
	selectionMade = false;
}

void MenuDialog::setText(wstring text) {
	dialogText->setText(text);
	firstSelectionPosition = dialogText->getPosition();
	firstSelectionPosition.x += dialogTextMargin.x;
	firstSelectionPosition.y += dialogText->getHeight() + dialogTextMargin.y;
	pointerPos = firstSelectionPosition;
	pointerPos.y += dialogText->getHeight() / 2;
	pointer->setPosition(pointerPos);
}

void MenuDialog::setDimensions(const Vector2& position, const Vector2& size) {
	DynamicDialog::setDimensions(position, size);

	dialogText->setScale(Vector2(1.25, 1.25));

	firstSelectionPosition = dialogText->getPosition();
	firstSelectionPosition.x += dialogTextMargin.x;
	firstSelectionPosition.y += dialogText->getHeight() + dialogTextMargin.y;

	pointerPos = firstSelectionPosition;
	pointerPos.y += dialogText->getHeight() / 2;

}



void MenuDialog::clearSelections() {
	selections.clear();
	selectionsActive.clear();
	selectionMade = false;
}

void MenuDialog::addSelection(wstring selection, bool enabled) {

	Vector2 newSize;

	Vector2 pos;
	size_t numSelections = selections.size();
	if (numSelections == 0) {
		pos = firstSelectionPosition;
		pointer->setPosition(pointerPos);
	} else {
		pos = selections[numSelections - 1]->getPosition();
		pos.y += (selections[numSelections - 1]->getHeight() + dialogTextMargin.y);

		for (const auto& selection : selections) {
			if (newSize.x < selection->getWidth() + dialogTextMargin.x * 2)
				newSize.x = selection->getWidth() + dialogTextMargin.x * 2;
		}
	}
	unique_ptr<TextLabel> newLabel;
	newLabel.reset(guiFactory->createTextLabel(pos, selection));

	if (newSize.x < newLabel->getWidth() + dialogTextMargin.x * 2)
		newSize.x = newLabel->getWidth() + dialogTextMargin.x * 2;

	if (!enabled)
		newLabel->setTint(Colors::Gray);
	selections.push_back(move(newLabel));
	selectionsActive.push_back(enabled);

	newSize.y = ((pos.y + selections[selections.size() - 1]->getHeight() + dialogTextMargin.y)
		- position.y) + dialogTextMargin.y;
	if (dialogText->getWidth() + dialogTextMargin.x > newSize.x)
		newSize.x = dialogText->getWidth() + dialogTextMargin.x;

	if (newSize.x > size.x || newSize.y > size.y) {
		setDimensions(position, newSize);
	}
}


void MenuDialog::update(double deltaTime) {
	if (!isShowing)
		return;
	DynamicDialog::update(deltaTime);


	//if (playerSlot->getStick()->lAxisY > 10
	if (playerSlot->getStick()->isDownPressed()
		//|| playerSlot->getStick()->bButtonStates[ControlButtons::SELECT]) {
		|| playerSlot->getStick()->selectButtonDown()) {

		repeatDelayTime += deltaTime;
		if (repeatDelayTime >= REPEAT_DELAY) {
			bool circle = false;
			do {
				++selectedItem;
				if (selectedItem >= selections.size()) {
					selectedItem = 0;
					if (circle) // we have infinite loop
						break;
					circle = true;

				}
			} while (!selectionsActive[selectedItem]);

			Vector2 newpos = selections[selectedItem]->getPosition();
			newpos.y += selections[selectedItem]->getHeight() / 2;
			pointer->setPosition(newpos);
			repeatDelayTime = 0;
		}
	//} else if (playerSlot->getStick()->lAxisY < -10) {
	} else if (playerSlot->getStick()->isUpPressed() ) {
		repeatDelayTime += deltaTime;
		if (repeatDelayTime >= REPEAT_DELAY) {
			bool circle = false;
			do {
				--selectedItem;
				if (selectedItem < 0) {
					selectedItem = selections.size() - 1;
					if (circle) // we have infinite loop
						break;
					circle = true;
				}
			} while (!selectionsActive[selectedItem]);

			Vector2 newpos = selections[selectedItem]->getPosition();
			newpos.y += selections[selectedItem]->getHeight() / 2;
			pointer->setPosition(newpos);
			repeatDelayTime = 0;
		}

	} else {
		repeatDelayTime = REPEAT_DELAY;
	}


	if (playerSlot->getStick()->aButtonPushed() || playerSlot->getStick()->startButtonPushed()) {

			// option selected
		selectionMade = true;

	} else if (playerSlot->getStick()->bButtonPushed()) {
		playerSlot->resetCharacterSelect();
		selectedItem = TitleItems::CANCEL;
		selectionMade = true;
		hide();
	}

	for (const auto& selection : selections)
		selection->update(deltaTime);

	pointer->update(deltaTime);
}


void MenuDialog::draw(SpriteBatch* batch) {

	if (!isShowing)
		return;

	DynamicDialog::draw(batch);

	for (const auto& selection : selections)
		selection->draw(batch);

	pointer->draw(batch);
}

void MenuDialog::show() {
	DynamicDialog::show();

	released = false;
}

void MenuDialog::hide() {
	DynamicDialog::hide();

}

USHORT MenuDialog::getSelected() {
	selectionMade = false;
	return selectedItem;
}

bool MenuDialog::currentPlayerIs(PlayerSlot* pSlot) {
	return playerSlot == pSlot;
}
