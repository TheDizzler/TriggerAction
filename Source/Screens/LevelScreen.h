#pragma once

#include "../DXTKGui/BaseGraphics/screen.h"
#include "../GameObjects/PlayerCharacter.h"
#include "../Managers/MapManager.h"


extern vector<Tangible*> hitboxesAll;


class LevelScreen : public Screen {
public:

	virtual ~LevelScreen();

	/** Don't need to use in LevelScreen! */
	virtual bool initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) override;
	virtual void setGameManager(GameManager* game) override;

	void loadMap(unique_ptr<Map> map);
	void reloadMap(unique_ptr<Map> map);

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void pause() override;
	virtual void controllerRemoved(size_t controllerSlot) override;
	virtual void newController(HANDLE joyHandle);

private:
	GameManager* game;

	unique_ptr<Map> map;

	vector<unique_ptr<PlayerCharacter>> pcs;


};