#include "../pch.h"
#include "LevelScreen.h"

vector<Tangible*> tangiblesAll;
vector<unique_ptr<PlayerCharacter>> pcs;
vector<Baddie*> baddies;
vector<Trigger*> triggersAll;

JammerManager LevelScreen::jammerMan;


LevelScreen::~LevelScreen() {
	pcs.clear();
	tangiblesAll.clear();
	baddies.clear();
	jammerMan.reset();
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
#include "../GameObjects/Characters/Chrono.h"
void LevelScreen::loadMap(unique_ptr<Map> newMap) {

	tangiblesAll.clear();
	pcs.clear();
	baddies.clear();
	jammerMan.reset();

	map.reset();
	map = move(newMap);

	map->calculateShadows();

	for (auto& tile : map->tangibles) {
		tangiblesAll.push_back(tile);
	}

	for (auto& baddie : map->baddies) {
		tangiblesAll.push_back(baddie.get());
		baddies.push_back(baddie.get());
	}

	for (auto& trigger : map->triggers) {
		triggersAll.push_back(trigger.get());
	}

	EventTrigger* start = map->start.get();
	Vector3 firstPStart = start->getHitbox()->position;
	firstPStart.x += start->getHitbox()->size.x / 2;
	firstPStart.y += start->getHitbox()->size.y;
	Vector3 secondPStart = start->getHitbox()->position;
	Vector3 thirdPStart = start->getHitbox()->position;
	thirdPStart.x += start->getHitbox()->size.y;

	Vector3 startPos[3] = {firstPStart, secondPStart, thirdPStart};
	int i = 0;
	for (const auto& slot : activeSlots) {

		if (slot->characterData->name == "Lucca") {
			unique_ptr<Lucca> newPC = make_unique<Lucca>(slot);
			tangiblesAll.push_back(newPC.get());
			newPC->setInitialPosition(startPos[i++]);

			pcStatusDialogs[slot->getPlayerSlotNumber()] =
				guiOverlay->createPCStatusDialog(
					guiFactory->getAssetSet("Menu BG 0"), slot->getPlayerSlotNumber());
			pcStatusDialogs[slot->getPlayerSlotNumber()]->loadPC(newPC.get());
			newPC->loadMap(map);
			pcs.push_back(move(newPC));

		} else if (slot->characterData->name == "Marle") {
			unique_ptr<Marle> newPC = make_unique<Marle>(slot);
			tangiblesAll.push_back(newPC.get());
			newPC->setInitialPosition(startPos[i++]);
			pcStatusDialogs[slot->getPlayerSlotNumber()] =
				guiOverlay->createPCStatusDialog(
					guiFactory->getAssetSet("Menu BG 0"), slot->getPlayerSlotNumber());
			pcStatusDialogs[slot->getPlayerSlotNumber()]->loadPC(newPC.get());
			newPC->loadMap(map);
			pcs.push_back(move(newPC));

		} else if (slot->characterData->name == "Chrono") {
			unique_ptr<Chrono> newPC = make_unique<Chrono>(slot);
			tangiblesAll.push_back(newPC.get());
			newPC->setInitialPosition(startPos[i++]);
			pcStatusDialogs[slot->getPlayerSlotNumber()] =
				guiOverlay->createPCStatusDialog(
					guiFactory->getAssetSet("Menu BG 0"), slot->getPlayerSlotNumber());
			pcStatusDialogs[slot->getPlayerSlotNumber()]->loadPC(newPC.get());
			newPC->loadMap(map);
			pcs.push_back(move(newPC));

		}
	}

	for (i; i < 3; ++i) {
		//CharacterData* characterData = gfxAssets->getNextCharacter(&currentCharacterNum);

	}

	guiOverlay->initializeLevelScreen(pcStatusDialogs);

	camera->setZoomToResolution();
	camera->centerOn(Vector2(256 / 2, 224 / 2));
}

void LevelScreen::reloadMap(unique_ptr<Map> newMap) {

	tangiblesAll.clear();
	baddies.clear();
	jammerMan.reset();
	triggersAll.clear();

	map.reset();
	map = move(newMap);

	map->calculateShadows();

	for (auto& tile : map->tangibles) {
		tangiblesAll.push_back(tile);
	}

	for (auto& baddie : map->baddies) {
		tangiblesAll.push_back(baddie.get());
		baddies.push_back(baddie.get());
	}

	for (auto& trigger : map->triggers) {
		triggersAll.push_back(trigger.get());
	}

	EventTrigger* start = map->start.get();
	Vector3 firstPStart = start->getHitbox()->position;
	firstPStart.x += start->getHitbox()->size.x / 2;
	firstPStart.y += start->getHitbox()->size.y;
	Vector3 secondPStart = start->getHitbox()->position;
	Vector3 thirdPStart = start->getHitbox()->position;
	thirdPStart.x += start->getHitbox()->size.y;

	Vector3 startPos[3] = {firstPStart, secondPStart, thirdPStart};
	int i = 0;

	for (auto& pc : pcs) {
		pc->reloadData(gfxAssets->getCharacterData(pc->name));
		pc->loadMap(map);
		pc->setInitialPosition(startPos[i++]);
		tangiblesAll.push_back(pc.get());

	}

	guiOverlay->initializeLevelScreen(pcStatusDialogs);

	camera->setZoomToResolution();
	camera->centerOn(Vector2(256 / 2, 224 / 2));
}


void LevelScreen::update(double deltaTime) {

	if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
		game->confirmExit();
	}

	map->update(deltaTime);

	bool gameOver = true;
	for (const auto& pc : pcs) {
		pc->update(deltaTime);
		if (gameOver && pc->isAlive)
			gameOver = false;
	}

	if (gameOver) {
		//game->loadMainMenu();
		GameEngine::showCustomDialog(activeSlots[0]->pauseDialog.get());
	}

	jammerMan.update(deltaTime);
}

void LevelScreen::draw(SpriteBatch * batch) {

	map->draw(batch);

	for (const auto& pc : pcs)
		pc->draw(batch);

	jammerMan.draw(batch);
}

void LevelScreen::pause() {

	game->confirmExit();
}


void LevelScreen::controllerRemoved(ControllerSocketNumber controllerSlot,
	PlayerSlotNumber slotNumber) {


	game->setPaused(true);

	guiOverlay->reportLostJoystick(controllerSlot);

}

void LevelScreen::newController(shared_ptr<Joystick> newStick) {
}
