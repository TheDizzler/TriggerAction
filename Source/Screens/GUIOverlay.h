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