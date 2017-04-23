#include "../pch.h"
#include "LevelScreen.h"

vector<const Hitbox*> hitboxesAll;



LevelScreen::~LevelScreen() {
	pcs.clear();
}

#include "../Engine/GameEngine.h"
bool LevelScreen::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) {

	//for (int i = 0; i < joystickSlots; ++i) {
	int i = 0;
	//for (const auto& joystick : joystickPorts) {
		//if (!joystick->getHandle())
			//continue;
	for (const auto& slot : playerSlots) {
		if (!slot->hasJoystick())
			continue;
		unique_ptr<PlayerCharacter> newPC = make_unique<PlayerCharacter>(slot);
		const CharacterData* dataSet = gfxAssets->getPlayerData(characters[i]);
		if (!dataSet)
			return false;
		newPC->initialize(dataSet, i);

		hitboxesAll.push_back(newPC->getHitbox());
		pcs.push_back(move(newPC));
		++i;
	}
	return true;
}

void LevelScreen::setGameManager(GameManager* gm) {

	game = gm;
}

void LevelScreen::loadMap(unique_ptr<Map> newMap) {

	map.reset();
	map = move(newMap);

	for (const auto& pc : pcs)
		pc->setInitialPosition();

	camera->centerOn(Vector2(256 / 2, 224 / 2));
}


void LevelScreen::update(double deltaTime) {

	if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
		game->confirmExit();
	}

	map->update(deltaTime);

	for (const auto& pc : pcs)
		pc->update(deltaTime);
}

void LevelScreen::draw(SpriteBatch * batch) {

	map->draw(batch);

	for (const auto& pc : pcs)
		pc->draw(batch);
}

void LevelScreen::pause() {

// open player menu

	game->confirmExit();
}


void LevelScreen::controllerRemoved(size_t controllerSlot) {


	game->setPaused(true);

	guiOverlay->reportLostJoystick(controllerSlot);

}

void LevelScreen::newController(HANDLE joyHandle) {
}
