#pragma once

#include "../DXTKGui/Controls/Dialog.h"
#include "../Engine/Joystick.h"

class GUIOverlay {
public:
	GUIOverlay(vector<shared_ptr<Joystick>> joysticks);
	//~GUIOverlay();


	void update(double deltaTime, shared_ptr<MouseController> mouse);
	void draw(SpriteBatch* batch);

	//void createDialog(wstring text, wstring title);

	void openLostControllerDialog(size_t playerNum);	
private:

	vector<shared_ptr<Joystick>> joysticks;
	unique_ptr<Dialog> controllerLostDialog;
};