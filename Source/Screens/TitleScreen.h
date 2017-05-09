#pragma once
#include "../DXTKGui/BaseGraphics/screen.h"
//#include "../Engine/Joystick.h"
#include "GUIOverlay.h"


class TitleScreen : public Screen {
public:
	//TitleScreen();
	virtual ~TitleScreen();


	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void setGameManager(GameManager* game) override;

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch * batch) override;

	virtual void pause() override;
	virtual void controllerRemoved(size_t controllerSlot) override;
	virtual void newController(HANDLE joyHandle);

private:

	enum ScreenState {
		START_MENU, CHARACTER_SELECT
	};
	ScreenState state = START_MENU;

	GameManager* game;

	unique_ptr<ControllerDialog> noControllerDialog;
	//unique_ptr<DynamicDialog> testDialog;

	unique_ptr<Button> quitButton;
	vector<unique_ptr<RectangleFrame>> frames;



	unique_ptr<Sprite> pendulum;
	float pendulumRotation;

	float angularVelocity = 0;

};