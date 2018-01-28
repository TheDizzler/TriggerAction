#pragma once

#include "../DXTKGui/BaseGraphics/screen.h"
#include "../GameObjects/Characters/PlayerCharacter.h"
#include "../GUIObjects//PCStatusDialog.h"
#include "../Managers/MapManager.h"
#include "../Managers/JammerManager.h"

//#define DEBUG_HITBOXES

extern vector<Tangible*> tangiblesAll;
extern vector<unique_ptr<PlayerCharacter>> pcs;
extern vector<Baddie*> baddies;
extern vector<Trigger*> triggersAll;

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
	virtual void controllerRemoved(ControllerSocketNumber controllerSlot,
		PlayerSlotNumber slotNumber) override;
	virtual void newController(shared_ptr<Joystick> newStick) override;

	static JammerManager jammerMan;
	
private:
	GameManager* game;

	shared_ptr<Map> map;

	unique_ptr<PCStatusDialog> pcStatusDialogs[MAX_PLAYERS];
	//unique_ptr<PCTechDialog> pcTechDialogs[MAX_PLAYERS];

	

};