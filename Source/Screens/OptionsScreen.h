#pragma once
#include "../DXTKGui/BaseGraphics/screen.h"
#include "GUIOverlay.h"

class OptionsScreen : public Screen {
public:
	virtual ~OptionsScreen();

	virtual bool initialize(
		ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void setGameManager(GameManager* game) override;
	

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void textureDraw(SpriteBatch* batch) override;

	virtual void pause() override;
	virtual void controllerRemoved(
		ControllerSocketNumber controllerSlot, PlayerSlotNumber slotNumber) override;
	virtual void newController(shared_ptr<Joystick> newStick) override;


private:

	GameManager* game;

	unique_ptr<TextLabel> attackLabel;
	unique_ptr<TextLabel> jumpLabel;
	unique_ptr<TextLabel> runLabel;
	unique_ptr<TextLabel> blockLabel;
};