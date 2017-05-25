#pragma once

#include "../GUIObjects/MenuDialog.h"
#include "../GUIObjects/PCSelectDialog.h"
#include "../GUIObjects//PCStatusDialog.h"
#include "../Engine/Joystick.h"

#include "../globals.h"


class GUIOverlay {
public:
	GUIOverlay();
	virtual ~GUIOverlay();

	void initializeTitleScreen(unique_ptr<PCSelectDialog> pcSelectDialogs[MAX_PLAYERS]);
	void initializeLevelScreen(unique_ptr<PCStatusDialog> pcStatusDialogs[MAX_PLAYERS]);


	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	void showMenu();

	void setDialogText(USHORT playerSlotNumber, wstring text);

	void reportLostJoystick(size_t playerSlotNumber); // currently does nothing

	unique_ptr<PCStatusDialog> createPCStatusDialog(
		shared_ptr<AssetSet> dialogImageSet,
		const USHORT playerNumber, const char_t* fontName = "Default Font");

	unique_ptr<PCSelectDialog> createPCSelectDialog(shared_ptr<AssetSet> dialogImageSet,
		const USHORT playerNumber, const char_t* fontName = "Default Font");

	/** Displayed by GUIOverlay but controlled by it's associated screen. */
	unique_ptr<MenuDialog> menuDialog;
private:

	/** The order of these is important!! */
	enum HUDDIALOG {
		ENEMIES, PLAYERSTATS, PLAYER1, PLAYER2, PLAYER3
	};
	DynamicDialog* hudDialogs[HUDDIALOG::PLAYER3 + 1];
	Vector2 dialogPositions[HUDDIALOG::PLAYER3 + 1];
	Vector2 dialogSize;
	unique_ptr<TextLabel> fpsLabel;


	unique_ptr<DynamicDialog> dummyDialog;


	vector<unique_ptr<Dialog>> lostJoyDialogs;
	vector<int> displayingLostJoys;



	unique_ptr<PCSelectDialog> createPCSelectDialog(shared_ptr<AssetSet> dialogImageSet,
		const Vector2& position, const Vector2& size, const char_t* fontName = "Default Font");
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