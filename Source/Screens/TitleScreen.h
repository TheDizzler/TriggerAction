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

	void reload();

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch * batch) override;

	virtual void pause() override;
	virtual void controllerRemoved(ControllerSocketNumber controllerSlot,
		PlayerSlotNumber slotNumber) override;
	virtual void newController(shared_ptr<Joystick> newStick) override;

private:

	enum ScreenState {
		START_MENU, CHARACTER_SELECT
	};
	ScreenState state = START_MENU;

	GameManager* game;

	PlayerSlotNumber playerWithMenuControl;
	unique_ptr<ControllerDialog> noControllerDialog;

	unique_ptr<Sprite> pendulum;
	float pendulumRotation;

	float angularVelocity = 0;

	unique_ptr<PCSelectDialog> pcSelectDialogs[MAX_PLAYERS];

};