#include "../pch.h"
#include "GUIOverlay.h"

const int TEXT_MARGIN = 5;

#include "../Engine/GameEngine.h"
GUIOverlay::GUIOverlay() {

	textLabel.reset(guiFactory->createTextLabel(Vector2(-100, -100)));

	for (auto& dialog : hudDialogs)
		dialog = guiFactory->createDialog(guiFactory->getAssetSet("Menu BG 1"));

	Vector2 textMeasure = textLabel->measureString(L"TECH");
	// should be enough to fit 4 lines of text + margin
	int windowHeight = textMeasure.y * 4 + TEXT_MARGIN;
	Vector2 pos, size;
	size = Vector2(Globals::WINDOW_WIDTH * 2 / 3, windowHeight);
	pos = Vector2(Globals::WINDOW_WIDTH - size.x, 0);
	hudDialogs[HUDDIALOG::PLAYERSTATS]->setDimensions(pos, size);
	pos = Vector2::Zero;
	hudDialogs[HUDDIALOG::ENEMIES]->setDimensions(pos, size);

	// should be wide enough to fit four letter word and the hand icon
	size.x = textMeasure.x + guiFactory->getAsset("Cursor Hand 1")->getWidth() + TEXT_MARGIN;
	hudDialogs[HUDDIALOG::PLAYER1]->setDimensions(pos, size);
	pos.x += size.x;
	hudDialogs[HUDDIALOG::PLAYER2]->setDimensions(pos, size);
	pos.x += size.x;
	hudDialogs[HUDDIALOG::PLAYER3]->setDimensions(pos, size);
}

GUIOverlay::~GUIOverlay() {
	lostJoyDialogs.clear();

}

void GUIOverlay::update(double deltaTime, shared_ptr<MouseController> mouse) {

	//for (const auto& dialog : hudDialogs)
		//dialog->update(deltaTime);

	for (const auto& dialog : lostJoyDialogs) {
		dialog->update(deltaTime);

	}
}

void GUIOverlay::draw(SpriteBatch* batch) {

	for (const auto& dialog : hudDialogs)
		dialog->draw(batch);

	for (const auto& dialog : lostJoyDialogs)
		dialog->draw(batch);
}

void GUIOverlay::setDialogText(USHORT playerSlot, wstring text) {

	hudDialogs[PLAYER1 + playerSlot + 1]->setText(text);
}

#include "../DXTKGui/StringHelper.h"
void GUIOverlay::reportLostJoystick(size_t controllerSlot) {

	/*for (int i : displayingLostJoys) {
		if (i == controllerSlot)
			return;
	}*/

	displayingLostJoys.push_back(controllerSlot);
	shared_ptr<Joystick> lostJoy = joysticks[controllerSlot];

	unique_ptr<ControllerDialog> joyLostDialog;
	size_t numDialogs = lostJoyDialogs.size();
	Vector2 dialogPos, dialogSize;
	if (numDialogs <= 0)
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 4, Globals::WINDOW_HEIGHT / 4);
	else
		dialogSize = Vector2(lostJoyDialogs[0]->getWidth(), lostJoyDialogs[0]->getHeight());


	dialogPos = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	dialogPos.y -= dialogSize.y / 2;

	if (numDialogs <= 0) {
		dialogPos.x -= dialogSize.x / 2;
	} else {
		for (const auto& dialog : lostJoyDialogs)
			dialog->moveBy(Vector2(-dialogSize.x / 2, 0));
		dialogPos.x += (dialogSize.x / 2) * (numDialogs - 1);
	}


	joyLostDialog = make_unique<ControllerDialog>(guiFactory.get());
	joyLostDialog->setDimensions(dialogPos, dialogSize);
	wostringstream title;
	title << L"Player " << lostJoy->slot;
	title << L"  has dropped." << endl;
	joyLostDialog->setTitle(title.str(), Vector2(1.2, 1.2));
	wostringstream wss;
	wss << StringHelper::convertCharStarToWCharT(lostJoy->pc->name.c_str()) << endl;
	wss << L"Waiting for controller...\n";
	joyLostDialog->setText(wss.str());
	joyLostDialog->show();

	lostJoyDialogs.push_back(move(joyLostDialog));
}



ControllerDialog::ControllerDialog(GUIFactory* guiF)
	: Dialog(guiF->getHWND(), false, true) {

	initializeControl(guiF, guiF->getMouseController());
	initialize(guiFactory->getAsset("White Pixel"));

}


void ControllerDialog::setDimensions(const Vector2& position, const Vector2& size,
	const int frameThickness) {

	Dialog::setDimensions(position, size, frameThickness);
}


void ControllerDialog::update(double deltaTime) {

	/*if (!isOpen)
		return;*/

	if (tempJoysticks.size() > 0) {

		if (first) {
			first = false;
			dialogOpenTime = 0;
			Dialog::setText(L"Push A to start!");
		}
		dialogOpenTime += deltaTime;
		if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
			dialogOpenTime = 0;

		}

	} else {

		if (!first) {
			first = true;
			dialogOpenTime = 0;
			ellipsisii = 16;
			setText(defaultText);
		}

		dialogOpenTime += deltaTime;
		if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
			dialogOpenTime = 0;
			if (ellipsisii++ > 5) {
				ellipsisii = 0;
				setText(defaultText);
			} else {
				wstring text = dialogText->getText();
				text += L".";
				Dialog::setText(text);
			}
		}
	}
	Dialog::update(deltaTime);
}

void ControllerDialog::setText(wstring text) {

	defaultText = text;
	Dialog::setText(text);
}
