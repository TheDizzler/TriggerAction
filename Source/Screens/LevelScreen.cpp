#include "../pch.h"
#include "LevelScreen.h"

vector<Tangible*> hitboxesAll;
vector<unique_ptr<PlayerCharacter>> pcs;


LevelScreen::~LevelScreen() {
	pcs.clear();
	hitboxesAll.clear();
}

#include "../Engine/GameEngine.h"
bool LevelScreen::initialize(ComPtr<ID3D11Device> device,
	shared_ptr<MouseController> mouse) {


	return true;
}

void LevelScreen::setGameManager(GameManager* gm) {

	game = gm;
}

#include "../GameObjects/Characters/Lucca.h"
#include "../GameObjects/Characters/Marle.h"
void LevelScreen::loadMap(unique_ptr<Map> newMap) {

	map.reset();
	map = move(newMap);

	for (const auto& slot : activeSlots) {

		if (slot->characterData->name == "Lucca") {
			unique_ptr<Lucca> newPC = make_unique<Lucca>(slot);
			hitboxesAll.push_back(newPC.get());
			newPC->setInitialPosition(Vector2(10, 100));
			pcs.push_back(move(newPC));
		} else if (slot->characterData->name == "Marle") {
			unique_ptr<Marle> newPC = make_unique<Marle>(slot);
			hitboxesAll.push_back(newPC.get());
			newPC->setInitialPosition(Vector2(10, 100));
			pcs.push_back(move(newPC));

		}
	}

	camera->setZoomToResolution();
	camera->centerOn(Vector2(256 / 2, 224 / 2));
}

void LevelScreen::reloadMap(unique_ptr<Map> newMap) {

	map.reset();
	map = move(newMap);



	for (const auto& pc : pcs) {
		hitboxesAll.push_back(pc.get());
		pc->reloadData(gfxAssets->getCharacterData(pc->name));
	}
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
