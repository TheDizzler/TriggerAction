#pragma once

#include "../DXTKGui/Controls/Dialog.h"
#include "../Engine/Joystick.h"

class GUIOverlay {
public:
	GUIOverlay(vector<shared_ptr<Joystick>> joysticks);
	~GUIOverlay();


	void update(double deltaTime, shared_ptr<MouseController> mouse);
	void draw(SpriteBatch* batch);

	//void createDialog(wstring text, wstring title);
	void reportLostJoystick(size_t controllerSlot);

private:

	vector<shared_ptr<Joystick>> joysticks;

	vector<unique_ptr<Dialog>> dialogs;
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
};