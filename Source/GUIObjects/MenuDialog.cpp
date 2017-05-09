#include "../pch.h"
#include "MenuDialog.h"


MenuDialog::MenuDialog(GUIFactory* guiF) : DynamicDialog(guiF, NULL) {

	pointer = make_unique<SelectionPointer>();
	pointer->load(guiFactory->getAsset("Cursor Hand 2"));
}

MenuDialog::~MenuDialog() {
	selections.clear();
}


void MenuDialog::setText(wstring text) {
	dialogText->setText(text);
	firstSelectionPosition = dialogText->getPosition();
	firstSelectionPosition.x += dialogTextMargin.x;
	firstSelectionPosition.y += dialogText->getHeight() + dialogTextMargin.y;
	pointerPos = firstSelectionPosition;
	pointerPos.y += dialogText->getHeight() / 2;
}

void MenuDialog::setDimensions(const Vector2& position, const Vector2& size) {
	DynamicDialog::setDimensions(position, size);

	dialogText->setScale(Vector2(1.25, 1.25));

	firstSelectionPosition = dialogText->getPosition();
	firstSelectionPosition.x += dialogTextMargin.x;
	firstSelectionPosition.y += dialogText->getHeight() + dialogTextMargin.y;

	pointerPos = firstSelectionPosition;
	pointerPos.y += dialogText->getHeight() / 2;
	//pointer->setPosition(pointerPos);

}

void MenuDialog::pairPlayerSlot(PlayerSlot* slot) {
	playerSlot = slot;
}

void MenuDialog::clearSelections() {
	selections.clear();
	selectionMade = false;
}

void MenuDialog::addSelection(wstring selection, bool enabled) {

	Vector2 newSize/* = Vector2(dialogText->getWidth() + dialogTextMargin.x * 2,
		dialogText->getHeight() + dialogTextMargin.y * 2)*/;

	Vector2 pos;
	size_t numSelections = selections.size();
	if (numSelections == 0)
		pos = firstSelectionPosition;
	else {
		pos = selections[numSelections - 1]->getPosition();
		pos.y += (selections[numSelections - 1]->getHeight() + dialogTextMargin.y);

		for (const auto& selection : selections) {
			if (newSize.x < selection->getWidth() + dialogTextMargin.x * 2)
				newSize.x = selection->getWidth() + dialogTextMargin.x * 2;
			//newSize.y += selection->getHeight() + dialogTextMargin.y;
		}
	}
	unique_ptr<TextLabel> newLabel;
	newLabel.reset(guiFactory->createTextLabel(pos, selection));

	if (newSize.x < newLabel->getWidth() + dialogTextMargin.x * 2)
		newSize.x = newLabel->getWidth() + dialogTextMargin.x * 2;
	////if (newSize.y + newLabel->getHeight() + dialogTextMargin.y > size.x)
	//newSize.y += newLabel->getHeight() + dialogTextMargin.y;

	if (!enabled)
		newLabel->setTint(Colors::Gray);
	selections.push_back(move(newLabel));

	pointer->setPosition(pointerPos);

	newSize.y = ((pos.y + selections[selections.size() - 1]->getHeight() + dialogTextMargin.y)
		- position.y) + dialogTextMargin.y;



	if (newSize.x > size.x || newSize.y > size.y) {
		setDimensions(position, newSize);
	}
}


void MenuDialog::update(double deltaTime) {
	if (!isShowing)
		return;
	DynamicDialog::update(deltaTime);


	if (playerSlot->getStick()->lAxisY > 10
		|| playerSlot->getStick()->bButtonStates[ControlButtons::SELECT]) {

		repeatDelayTime += deltaTime;
		if (repeatDelayTime >= REPEAT_DELAY) {
			++selectedItem;
			if (selectedItem >= selections.size())
				selectedItem = 0;
			Vector2 newpos = selections[selectedItem]->getPosition();
			newpos.y += selections[selectedItem]->getHeight() / 2;
			pointer->setPosition(newpos);
			repeatDelayTime = 0;
		}
	} else if (playerSlot->getStick()->lAxisY < -10) {

		repeatDelayTime += deltaTime;
		if (repeatDelayTime >= REPEAT_DELAY) {
			--selectedItem;
			if (selectedItem < 0)
				selectedItem = selections.size() - 1;
			Vector2 newpos = selections[selectedItem]->getPosition();
			newpos.y += selections[selectedItem]->getHeight() / 2;
			pointer->setPosition(newpos);
			repeatDelayTime = 0;
		}

	} else {
		repeatDelayTime = REPEAT_DELAY;
	}

	if (playerSlot->getStick()->bButtonStates[ControlButtons::A] ||
		playerSlot->getStick()->bButtonStates[ControlButtons::START]) {

		if (released) {
		// option selected
			selectionMade = true;
		}
	} else {
		released = true;
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

USHORT MenuDialog::getSelected() {
	selectionMade = false;
	return selectedItem;
}
