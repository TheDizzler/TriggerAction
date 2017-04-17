#include "../pch.h"
#include "GUIOverlay.h"

#include "../Engine/GameEngine.h"
GUIOverlay::GUIOverlay(vector<shared_ptr<Joystick>> joys) {

	joysticks = joys;

}

GUIOverlay::~GUIOverlay() {
	dialogs.clear();
}

void GUIOverlay::update(double deltaTime, shared_ptr<MouseController> mouse) {

	for (const auto& dialog : dialogs) {
		dialog->update(deltaTime);

	}
}

void GUIOverlay::draw(SpriteBatch* batch) {

	for (const auto& dialog : dialogs)
		dialog->draw(batch);
}

#include "../DXTKGui/StringHelper.h"
void GUIOverlay::reportLostJoystick(size_t controllerSlot) {

	for (int i : displayingLostJoys) {
		if (i == controllerSlot)
			return;
	}

	displayingLostJoys.push_back(controllerSlot);
	shared_ptr<Joystick> lostJoy = joysticks[controllerSlot];

	unique_ptr<ControllerDialog> joyLostDialog;
	size_t numDialogs = dialogs.size();
	Vector2 dialogPos, dialogSize;
	if (numDialogs <= 0)
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 4, Globals::WINDOW_HEIGHT / 4);
	else
		dialogSize = Vector2(dialogs[0]->getWidth(), dialogs[0]->getHeight());


	dialogPos = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	dialogPos.y -= dialogSize.y / 2;

	if (numDialogs <= 0) {
		dialogPos.x -= dialogSize.x / 2;
	} else {
		for (const auto& dialog : dialogs)
			dialog->moveBy(Vector2(-dialogSize.x / 2, 0));
		dialogPos.x += (dialogSize.x / 2) * (numDialogs - 1);
	}


	joyLostDialog = make_unique<ControllerDialog>(guiFactory.get());
	//guiFactory->createDialog(dialogPos, dialogSize, false, true, 10);
	joyLostDialog->setDimensions(dialogPos, dialogSize);
	wostringstream title;
	title << L"Player " << lostJoy->slot;
	title << L"  has dropped." << endl;
	joyLostDialog->setTitle(title.str(), Vector2(1.2, 1.2));
	wostringstream wss;
	wss << StringHelper::convertCharStarToWCharT(lostJoy->pc->name.c_str()) << endl;
	wss << L"Waiting for controller...\n";
	/*wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n";
	wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n";
	wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n";
	wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n"; wss << L"eaeea\n";*/
	joyLostDialog->setText(wss.str());
	joyLostDialog->show();

	dialogs.push_back(move(joyLostDialog));
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

	dialogOpenTime += deltaTime;
	if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
		dialogOpenTime = 0;
		if (ellipsisii++ > 6) {
			ellipsisii = 0;
			setText(defaultText);
		} else {
			wstring text = dialogText->getText();
			text += L".";
			Dialog::setText(text);
		}
	}
	Dialog::update(deltaTime);
}

void ControllerDialog::setText(wstring text) {

	defaultText = text;
	Dialog::setText(text);
}
