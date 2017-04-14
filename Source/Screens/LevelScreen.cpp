#include "../pch.h"
#include "LevelScreen.h"

vector<const Hitbox*> hitboxesAll;


LevelScreen::LevelScreen(vector<shared_ptr<Joystick>> joys) {
	joysticks = joys;
}

LevelScreen::~LevelScreen() {
	pcs.clear();
}

#include "../Engine/GameEngine.h"
bool LevelScreen::initialize(ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse) {

	for (int i = 0; i < joysticks.size(); ++i) {
		unique_ptr<PlayerCharacter> newPC = make_unique<PlayerCharacter>(joysticks[i]);
		const CharacterData* dataSet = gfxAssets->getPlayerData(characters[i]);
		if (!dataSet)
			return false;
		newPC->initialize(dataSet, i);

		hitboxesAll.push_back(newPC->getHitbox());
		pcs.push_back(move(newPC));
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
}


void LevelScreen::update(double deltaTime, shared_ptr<MouseController> mouse) {

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
	game->confirmExit();
}
