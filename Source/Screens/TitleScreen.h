#pragma once
#include "../DXTKGui/BaseGraphics/screen.h"
#include "../Engine/Joystick.h"
#include "GUIOverlay.h"


class TitleScreen : public Screen {
public:
	//TitleScreen();
	~TitleScreen();


	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void setGameManager(GameManager* game) override;

	virtual void update(double deltaTime, shared_ptr<MouseController> mouse) override;
	virtual void draw(SpriteBatch * batch) override;

	virtual void pause() override;
	virtual void controllerRemoved(size_t controllerSlot) override;
	virtual void newController(HANDLE joyHandle);

private:
	GameManager* game;
	
	unique_ptr<ControllerDialog> noControllerDialog;
	unique_ptr<ImageDialog> testDialog;

	unique_ptr<Button> quitButton;

	
	unique_ptr<Sprite> pendulum;
	float pendulumRotation;
	
	float angularVelocity = 0;

};