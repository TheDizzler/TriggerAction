#pragma once
#include "../DXTKGui/BaseGraphics/screen.h"
#include "../Engine/Joystick.h"


class TitleScreen : public Screen {
public:
	TitleScreen(vector<shared_ptr<Joystick>> joysticks);
	~TitleScreen();


	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void setGameManager(GameManager* game) override;

	virtual void update(double deltaTime, shared_ptr<MouseController> mouse) override;
	virtual void draw(SpriteBatch * batch) override;

	virtual void pause() override;
	virtual void controllerRemoved() override;

private:
	GameManager* game;
	vector<shared_ptr<Joystick>> joysticks;
	unique_ptr<Dialog> noControllerDialog;

	unique_ptr<Button> quitButton;

	
	unique_ptr<Sprite> pendulum;

};