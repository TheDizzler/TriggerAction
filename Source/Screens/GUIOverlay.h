#pragma once

#include "../GUIObjects/PCSelectDialog.h"
#include "../Engine/Joystick.h"

class GUIOverlay {
public:
	GUIOverlay();
	~GUIOverlay();


	void update(double deltaTime, shared_ptr<MouseController> mouse);
	void draw(SpriteBatch* batch);

	void setDialogText(USHORT playerSlot, wstring text);

	void reportLostJoystick(size_t controllerSlot);
	void controllerRemoved(size_t controllerSlot);
	void unclaimedJoystickRemoved(JoyData* joyData);
	int controllerWaiting(JoyData* joyData);
	void controllerAccepted(JoyData* joyData);

	unique_ptr<PCSelectDialog> createPCDialog(shared_ptr<AssetSet> dialogImageSet,
		const Vector2& position, const Vector2& size,
		const char_t* pcName, const char_t* fontName = "Default Font");

private:

	/** The order of these is important!! */
	enum HUDDIALOG {
		ENEMIES, PLAYERSTATS, PLAYER1, PLAYER2, PLAYER3
	};
	unique_ptr<DynamicDialog> hudDialogs[HUDDIALOG::PLAYER3 + 1];

	unique_ptr<TextLabel> fpsLabel;

	vector<JoyData*> waitingForInput;
	vector<unique_ptr<Dialog>> lostJoyDialogs;

	vector<int> displayingLostJoys;
};


class ControllerDialog : public PromptDialog {
public:
	ControllerDialog(GUIFactory* guiFactory);

	virtual void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 10);

	virtual void update(double deltaTime) override;


	virtual void setText(wstring text) override;

private:
	wstring defaultText;
	double dialogOpenTime = 0;
	const double CONTROLLER_WAIT_TIME = 1.0;
	int ellipsisii = 16;
	bool first = true;
};