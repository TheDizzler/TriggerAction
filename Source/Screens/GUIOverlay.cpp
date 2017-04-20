#include "../pch.h"
#include "GUIOverlay.h"

const int TEXT_MARGIN = 5;

#include "../Engine/GameEngine.h"
GUIOverlay::GUIOverlay() {

	unique_ptr<TextLabel> textLabel;
	textLabel.reset(guiFactory->createTextLabel(Vector2(-100, -100)));

	for (auto& dialog : hudDialogs)
		dialog = guiFactory->createDialog(guiFactory->getAssetSet("Menu BG 0"));

	Vector2 textMeasure = textLabel->measureString(L"TECH");
	// should be enough to fit 4 lines of text + margin
	int windowHeight = textMeasure.y * 4 + TEXT_MARGIN;
	Vector2 pos, size;
	size = Vector2(Globals::WINDOW_WIDTH * 2 / 3, windowHeight);
	pos = Vector2(Globals::WINDOW_WIDTH - size.x, 16);
	hudDialogs[HUDDIALOG::PLAYERSTATS]->setDimensions(pos, size);
	pos = Vector2(0, 16);
	hudDialogs[HUDDIALOG::ENEMIES]->setDimensions(pos, size);

	// should be wide enough to fit four letter word and the hand icon
	size.x = textMeasure.x * 2 + guiFactory->getAsset("Cursor Hand 1")->getWidth() + TEXT_MARGIN;
	pos.y = 16;
	hudDialogs[HUDDIALOG::PLAYER1]->setDimensions(pos, size);
	pos.x += size.x;
	hudDialogs[HUDDIALOG::PLAYER2]->setDimensions(pos, size);
	pos.x += size.x;
	hudDialogs[HUDDIALOG::PLAYER3]->setDimensions(pos, size);


	fpsLabel.reset(guiFactory->createTextLabel(Vector2(Globals::WINDOW_WIDTH - 250, 20)));
	fpsLabel->setTint(Colors::Black);
	fpsLabel->setScale(Vector2(.5, .5));
	fpsLabel->setLayerDepth(1);
}

GUIOverlay::~GUIOverlay() {
	lostJoyDialogs.clear();

}

int frameCount = 0;
double fpsUpdateTime = 5;
const double FPS_UPDATE_TIME = 1;
void GUIOverlay::update(double deltaTime, shared_ptr<MouseController> mouse) {

	fpsUpdateTime += deltaTime;
	++frameCount;
	if (fpsUpdateTime >= FPS_UPDATE_TIME) {

		wostringstream wss;
		wss << "frameCount: " << frameCount << " fpsUpdateTime: " << fpsUpdateTime << endl;
		wss << "fps: " << frameCount / fpsUpdateTime;
		fpsLabel->setText(wss);

		fpsUpdateTime = 0;
		frameCount = 0;
	}

	for (const auto& joy : waitingForInput)
		if (joy->joystick->bButtonStates[0]) {
			//joy->slot = 0;
			joy->finishFlag = true;
		}


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


	fpsLabel->draw(batch);
}

void GUIOverlay::setDialogText(USHORT playerSlot, wstring text) {

	hudDialogs[PLAYER1 + playerSlot]->setText(text);
	hudDialogs[PLAYER1 + playerSlot]->show();
}

#include "../DXTKGui/StringHelper.h"
void GUIOverlay::reportLostJoystick(size_t controllerSlot) {

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

void GUIOverlay::controllerRemoved(size_t controllerSlot) {

	wstringstream wss;
	wss << "controller " << controllerSlot << " removed" << endl;
	OutputDebugString(wss.str().c_str());

	hudDialogs[PLAYER1 + controllerSlot]->close();

}


int GUIOverlay::controllerWaiting(JoyData* joyData) {

	for (int i = 0; i < 2; ++i)
		if (!hudDialogs[PLAYER1 + i]->isShowing()) {
			hudDialogs[PLAYER1 + i]->show();
			hudDialogs[PLAYER1 + i]->setText(L"Push A\nto begin!");
			waitingForInput.push_back(joyData);
			return i;

		}

	return -1;
}


void GUIOverlay::unclaimedJoystickRemoved(JoyData* joyData) {

	for (int i = 0; i < waitingForInput.size(); ++i) {
		if (waitingForInput[i] == joyData) {
			swap(waitingForInput[i], waitingForInput.back());
			waitingForInput.pop_back();
		}
	}

	controllerRemoved(joyData->tempSlot);
}


void GUIOverlay::controllerAccepted(JoyData* joyData) {

	for (int i = 0; i < waitingForInput.size(); ++i) {
		if (waitingForInput[i] == joyData) {
			swap(waitingForInput[i], waitingForInput.back());
			waitingForInput.pop_back();
		}
	}

	wostringstream ws;
	ws << L"Player " << (joyData->joystick->slot + 1);
	guiOverlay->setDialogText(joyData->joystick->slot, ws.str());
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

	if (!isOpen)
		return;

	/*if (tempJoysticks.size() > 0) {

		if (first) {
			first = false;
			dialogOpenTime = 0;
			Dialog::setText(L"Push A to start!");
		}
		dialogOpenTime += deltaTime;
		if (dialogOpenTime > CONTROLLER_WAIT_TIME) {
			dialogOpenTime = 0;

		}

	} else {*/

	/*if (!first) {
		first = true;
		dialogOpenTime = 0;
		ellipsisii = 16;
		setText(defaultText);
	}*/

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
//}
	Dialog::update(deltaTime);
}

void ControllerDialog::setText(wstring text) {

	defaultText = text;
	Dialog::setText(text);
}
