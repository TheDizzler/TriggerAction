#pragma once

#include "../GUIObjects/MenuDialog.h"
#include "../GUIObjects/PCSelectDialog.h"
#include "../Engine/Joystick.h"

#include "../globals.h"


class GUIOverlay {
public:
	GUIOverlay();
	virtual ~GUIOverlay();

	void initializeTitleScreen();

	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	void showMenu();

	void setDialogText(USHORT playerSlotNumber, wstring text);

	void reportLostJoystick(size_t playerSlotNumber);

	
	//void readyPCSelect(shared_ptr<PlayerSlot> playerSlot);

	//unique_ptr<TextLabel> fps2Label;

	/** Displayed by GUIOverlay but controlled by it's associated screen. */
	unique_ptr<MenuDialog> menuDialog;
private:

	/*int numPCsAvailable = 2;
	size_t nextAvaiablePC = 0;*/

	/** The order of these is important!! */
	enum HUDDIALOG {
		ENEMIES, PLAYERSTATS, PLAYER1, PLAYER2, PLAYER3
	};
	unique_ptr<PCSelectDialog> hudDialogs[HUDDIALOG::PLAYER3 + 1];

	
	unique_ptr<TextLabel> fpsLabel;
	


	//CRITICAL_SECTION cs_selectingPC;
	////vector<JoyData*> waitingForInput;

	vector<unique_ptr<Dialog>> lostJoyDialogs;
	vector<int> displayingLostJoys;

	

	unique_ptr<PCSelectDialog> createPCDialog(shared_ptr<AssetSet> dialogImageSet,
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