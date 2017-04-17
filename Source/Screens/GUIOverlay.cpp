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

	for (const auto& dialog : dialogs)
		dialog->update(deltaTime);
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

	unique_ptr<Dialog> joyLostDialog;
	size_t numDialogs = dialogs.size();
	Vector2 dialogPos, dialogSize;
	dialogSize = Vector2(Globals::WINDOW_WIDTH / 4, Globals::WINDOW_HEIGHT / 4);
	dialogPos = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	dialogPos.y -= dialogSize.y / 2;
	if (numDialogs <= 0) {
		dialogPos.x -= dialogSize.x / 2;

	} else {
		for (const auto& dialog : dialogs)
			dialog->moveBy(Vector2(-dialogSize.x / 2, 0));
		dialogPos.x += (dialogSize.x / 2) * (numDialogs - 1);
	}


	joyLostDialog = guiFactory->createDialog(dialogPos, dialogSize, false, true, 10);
	wostringstream wss;
	wss << L"Player " << lostJoy->slot << L" - ";
	wss << StringHelper::convertCharStarToWCharT(lostJoy->pc->name.c_str()) << L" - has dropped." << endl;
	wss << L"Waiting for controller...";
	joyLostDialog->setText(wss.str());
	joyLostDialog->show();

	dialogs.push_back(move(joyLostDialog));
}

