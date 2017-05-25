#include "../pch.h"
#include "GameManager.h"

Keyboard::KeyboardStateTracker keyTracker;
unique_ptr<GUIOverlay> guiOverlay;

GameManager::GameManager(GameEngine* gmngn) {

	gameEngine = gmngn;
}

GameManager::~GameManager() {
	mapFiles.clear();
}

#include "../Engine/GameEngine.h"
bool GameManager::initializeGame(HWND hwnd, ComPtr<ID3D11Device> dvc,
	shared_ptr<MouseController> ms) {

	device = dvc;
	mouse = ms;

	{
		Vector2 dialogPos, dialogSize;
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
		dialogPos = dialogSize;
		dialogPos.x -= dialogSize.x / 2;
		dialogPos.y -= dialogSize.y / 2;

		exitDialog.reset(guiFactory->createDialog(dialogPos, dialogSize, true, true, 10));
		//exitDialog->setDimensions(dialogPos, dialogSize);
		exitDialog->setTint(Color(0, .5, 1, 1));
		exitDialog->setTitle(L"Exit Game?", Vector2(1, 1), "BlackCloak");
		//exitDialog->setTitleAreaDimensions(Vector2(0, 150));
		exitDialog->setText(L"Really End The Trigger Action?");
		unique_ptr<Button> quitButton;
		quitButton.reset(guiFactory->createButton());
		quitButton->setActionListener(new OnClickListenerDialogQuitButton(this));
		quitButton->setText(L"Time Out");
		exitDialog->setConfirmButton(move(quitButton));
		exitDialog->setCancelButton(L"Keep Triggering!");
		exitDialog->setCancelOnClickListener(new CancelDialogButton(gameEngine, exitDialog.get()));
		//exitDialog->setMatrixFunction([&]()-> Matrix { return camera->translationMatrix(); });
		exitDialog->setOpenTransition(
			//new TransitionEffects::SpinGrowTransition(.5));
			//new TransitionEffects::SplitTransition(25));
		//new TransitionEffects::BlindsTransition(.25, false, true));
		//new TransitionEffects::TrueGrowTransition(exitDialog.get(), Vector2(.001, .001), Vector2(1, 1), 10));
		/*new TransitionEffects::SlideAndGrowTransition(
		Vector2(-200, -200), exitDialog->getPosition(),
		Vector2(.001, .001), Vector2(1, 1)));*/
			new TransitionEffects::GrowTransition(exitDialog.get(),
				Vector2(.0001, 0001), Vector2(1, 1), 12));
		/*new TransitionEffects::SlideTransition(
		Vector2(-200, -200), exitDialog->getPosition()));*/

		//exitDialog->setCloseTransition(
		/*new TransitionEffects::ShrinkTransition(
		Vector2(1, 1), Vector2(.001, .001)));*/
	}



	guiOverlay = make_unique<GUIOverlay>();


	mapManifest = make_unique<xml_document>();
	if (!mapManifest->load_file(Globals::mapManifestFile)) {
		wostringstream wss;
		wss << "Error trying to read " << Globals::mapManifestFile << ".";
		GameEngine::showErrorDialog(wss.str(), L"Error reading Map Manifest");
		return false;
	}

	xml_node mapRoot = mapManifest->child("root");

	mapsDir = mapRoot.attribute("dir").as_string();
	for (xml_node mapNode : mapRoot.children("map")) {
		const pugi::char_t* name = mapNode.attribute("name").as_string();
		string file = mapsDir + mapNode.attribute("file").as_string();
		mapFiles[name] = file;
	}

	mapParser = make_unique<MapParser>(device);

	levelScreen = make_unique<LevelScreen>();
	levelScreen->setGameManager(this);
	if (!levelScreen->initialize(device, mouse)) {
		GameEngine::showErrorDialog(L"Level Screen failed to initialize", L"Error initializing level");
		return false;
	}

	if (true) {

		titleScreen = make_unique<TitleScreen>();
		titleScreen->setGameManager(this);
		titleScreen->initialize(device, mouse);
		currentScreen = titleScreen.get();
	} else {


		string level = "Test Square C";
		string mFile = mapFiles[level];
		if (!loadLevel(mFile.c_str())) {
			GameEngine::showErrorDialog(L"Map failed to load", L"Error in GameManager::initializeGame()");
			return false;
		}
		currentScreen = levelScreen.get();

	}
	
	ShowCursor(false);

	return true;
}


void GameManager::update(double deltaTime) {

	auto state = Keyboard::Get().GetState();
	keyTracker.Update(state);

	currentScreen->update(deltaTime);

}


void GameManager::draw(SpriteBatch * batch) {

	currentScreen->draw(batch);

}


void GameManager::startGame() {

	if (!levelScreen->initialize(device, mouse)) {
		GameEngine::showErrorDialog(L"Failed to load Level Screen", L"Gaah");
		return;
	}
	currentScreen = levelScreen.get();
}


bool GameManager::loadLevel(const pugi::char_t* levelName) {

	string mFile = mapFiles[levelName];

	mapDoc.reset(new pugi::xml_document());
	if (!mapDoc->load_file(mFile.c_str())) {
		wostringstream wss;
		wss << "Error trying to read " << mFile.c_str() << ".";
		GameEngine::showErrorDialog(wss.str(), L"Error reading map file");
		return false;
	}

	if (!mapParser->parseMap(mapDoc->child("map"), mapsDir)) {
		GameEngine::showErrorDialog(L"Map failed to load",
			L"Error in GameManager::loadLevel()");
		return false;
	}

	levelScreen->loadMap(mapParser->getMap());

	currentScreen = levelScreen.get();
	return true;
}


bool GameManager::reloadLevel(const pugi::char_t* levelName) {

	hitboxesAll.clear();

	string mFile = mapFiles[levelName];

	mapDoc.reset(new pugi::xml_document());
	if (!mapDoc->load_file(mFile.c_str())) {
		wostringstream wss;
		wss << "Error trying to read " << mFile.c_str() << ".";
		GameEngine::showErrorDialog(wss.str(), L"Error reading map file");
		return false;
	}

	if (!mapParser->parseMap(mapDoc->child("map"), mapsDir)) {
		GameEngine::showErrorDialog(L"Map failed to load",
			L"Error in GameManager::loadLevel()");
		return false;
	}

	levelScreen->reloadMap(mapParser->getMap());

	return true;
}


void GameManager::loadMainMenu() {

	lastScreen = currentScreen;
	currentScreen = levelScreen.get();

}

void GameManager::controllerRemoved(size_t controllerSlot) {

	currentScreen->controllerRemoved(controllerSlot);
}

void GameManager::newController(HANDLE joyHandle) {
	currentScreen->newController(joyHandle);
}

void GameManager::controllerAccepted(HANDLE handle) {
	//gameEngine->controllerAccepted(handle);
}


void GameManager::setPaused(bool paused) {
	gameEngine->paused = paused;
}

void GameManager::pause() {

	if (currentScreen != NULL)
		currentScreen->pause();
}

void GameManager::confirmExit() {

	setPaused(true);
	if (!exitDialog->isOpen()) {
		GameEngine::showDialog = exitDialog.get();
		exitDialog->show();
		mouse->show();
	}
}


void GameManager::exit() {
	gameEngine->exit();
}


vector<ComPtr<IDXGIAdapter>> GameManager::getAdapterList() {
	return gameEngine->getAdapterList();
}

vector<ComPtr<IDXGIOutput>> GameManager::getDisplayList() {
	return gameEngine->getDisplayList();
}

vector<ComPtr<IDXGIOutput>> GameManager::getDisplayListFor(size_t displayIndex) {
	return gameEngine->getDisplayListFor(displayIndex);
}

vector<ComPtr<IDXGIOutput>> GameManager::getDisplayListFor(
	ComPtr<IDXGIAdapter> adapter) {
	return gameEngine->getDisplayListFor(adapter);
}

vector<DXGI_MODE_DESC> GameManager::getDisplayModeList(size_t displayIndex) {
	return gameEngine->getDisplayModeList(displayIndex);
}

vector<DXGI_MODE_DESC> GameManager::getDisplayModeList(ComPtr<IDXGIOutput> display) {
	return gameEngine->getDisplayModeList(display);
}


bool GameManager::setAdapter(size_t adapterIndex) {
	return gameEngine->setAdapter(adapterIndex);
}

bool GameManager::setDisplayMode(size_t displayModeIndex) {
	return gameEngine->changeDisplayMode(displayModeIndex);
}

bool GameManager::setFullScreen(bool isFullScreen) {
	return gameEngine->setFullScreen(isFullScreen);
}


size_t GameManager::getSelectedAdapterIndex() {
	return gameEngine->getSelectedAdapterIndex();
}

size_t GameManager::getSelectedDisplayIndex() {
	return gameEngine->getSelectedDisplayIndex();
}

size_t GameManager::getSelectedDisplayModeIndex() {
	return gameEngine->getSelectedDisplayModeIndex();
}



void CancelDialogButton::onClick(Button * button) {
	dialog->hide();
	engine->mouse->hide();
	engine->paused = false;
}

void CancelDialogButton::onPress(Button * button) {
}

void CancelDialogButton::onHover(Button * button) {
}
