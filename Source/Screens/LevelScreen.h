#pragma once

#include "../DXTKGui/BaseGraphics/screen.h"
#include "../GameObjects/PlayerCharacter.h"
#include "../Managers/MapManager.h"

static const char_t* characters[] = {"Marle", "Frog"};

extern vector<const Hitbox*> hitboxesAll;


class LevelScreen : public Screen {
public:



	//LevelScreen(vector<shared_ptr<Joystick>> joysticks);
	~LevelScreen();

	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void setGameManager(GameManager* game) override;

	void loadMap(unique_ptr<Map> map);

	virtual void update(double deltaTime, shared_ptr<MouseController> mouse) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void pause() override;
	virtual void controllerRemoved(size_t controllerSlot) override;
	virtual void newController(HANDLE joyHandle);

private:
	GameManager* game;

	unique_ptr<Map> map;

	//vector<shared_ptr<Joystick>> joysticks;
	vector<unique_ptr<PlayerCharacter>> pcs;


};