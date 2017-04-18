#pragma once

#include "../DXTKGui/Controls/Dialog.h"
#include "../Engine/Joystick.h"

class GUIOverlay {
public:
	GUIOverlay();
	~GUIOverlay();


	void update(double deltaTime, shared_ptr<MouseController> mouse);
	void draw(SpriteBatch* batch);

	void setDialogText(USHORT playerSlot, wstring text);
	
	void reportLostJoystick(size_t controllerSlot);

private:
	
	/** The order of these is important!! */
	enum HUDDIALOG {ENEMIES, PLAYERSTATS, PLAYER1, PLAYER2, PLAYER3};
	unique_ptr<ImageDialog> hudDialogs[HUDDIALOG::PLAYER3 + 1];

	unique_ptr<TextLabel> textLabel;
	vector<unique_ptr<Dialog>> lostJoyDialogs;

	vector<int> displayingLostJoys;
};


class ControllerDialog : public Dialog {
public:
	ControllerDialog(GUIFactory* guiFactory);

	virtual void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 10) override;

	virtual void update(double deltaTime) override;


	virtual void setText(wstring text) override;

private:
	wstring defaultText;
	double dialogOpenTime = 0;
	const double CONTROLLER_WAIT_TIME = 1.0;
	int ellipsisii = 16;
	bool first = true;
};