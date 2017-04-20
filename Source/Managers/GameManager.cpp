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
bool GameManager::initializeGame(HWND hwnd, ComPtr<ID3D11Device> dvc, shared_ptr<MouseController> ms/*,
	vector<shared_ptr<Joystick>> joysticks*/) {

	device = dvc;
	mouse = ms;

	{
		Vector2 dialogPos, dialogSize;
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
		dialogPos = dialogSize;
		dialogPos.x -= dialogSize.x / 2;
		dialogPos.y -= dialogSize.y / 2;

		exitDialog = guiFactory->createDialog(dialogPos, dialogSize, true, true, 10);
		//exitDialog->setDimensions(dialogPos, dialogSize);
		exitDialog->setTint(Color(0, .5, 1, 1));
		exitDialog->setTitle(L"Exit Test?", Vector2(1, 1), "BlackCloak");
		//exitDialog->setTitleAreaDimensions(Vector2(0, 150));
		exitDialog->setText(L"Really Quit The Test Project?");
		unique_ptr<Button> quitButton;
		quitButton.reset(guiFactory->createButton());
		quitButton->setOnClickListener(new OnClickListenerDialogQuitButton(this));
		quitButton->setText(L"Quit");
		exitDialog->setConfirmButton(move(quitButton));
		exitDialog->setCancelButton(L"Keep Testing!");
		//exitDialog->setMatrixFunction([&]()-> Matrix { return camera->translationMatrix(); });
		//exitDialog->open();
		exitDialog->setOpenTransition(
			/*new TransitionEffects::SpinGrowTransition(exitDialog.get(), .5));*/
			new TransitionEffects::SplitTransition(exitDialog.get(), 25));
		//new TransitionEffects::BlindsTransition(exitDialog.get(), .25, false, true));
		/*new TransitionEffects::TrueGrowTransition(exitDialog.get(),
		Vector2(.001, .001), Vector2(1, 1)));*/
		/*new TransitionEffects::SlideAndGrowTransition(
		Vector2(-200, -200), exitDialog->getPosition(),
		Vector2(.001, .001), Vector2(1, 1)));*/
		/*new TransitionEffects::GrowTransition(
		Vector2(.0001, 0001), Vector2(1, 1)));*/
		/*new TransitionEffects::SlideTransition(
		Vector2(-200, -200), exitDialog->getPosition()));*/

		//exitDialog->setCloseTransition(
		/*new TransitionEffects::ShrinkTransition(
		Vector2(1, 1), Vector2(.001, .001)));*/
		//exitDialog->close();
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
	mouse->loadMouseIcon(guiFactory.get(), "Mouse Icon");
	ShowCursor(false);

	return true;
}


void GameManager::update(double deltaTime, shared_ptr<MouseController> mouse) {

	auto state = Keyboard::Get().GetState();
	keyTracker.Update(state);

	currentScreen->update(deltaTime, mouse);

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


bool GameManager::loadLevel(const pugi::char_t* file) {

	mapDoc.reset(new pugi::xml_document());
	if (!mapDoc->load_file(file)) {
		wostringstream wss;
		wss << "Error trying to read " << file << ".";
		GameEngine::showErrorDialog(wss.str(), L"Error reading map file");
		return false;
	}

	if (!mapParser->parseMap(mapDoc->child("map"), mapsDir)) {
		return false;
	}
	levelScreen->loadMap(mapParser->getMap());
	return true;
}


void GameManager::loadMainMenu() {

	mouse->loadMouseIcon(guiFactory.get(), "Mouse Icon");
	mouse->setRotation(0);

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
	if (!exitDialog->isShowing()) {
		GameEngine::showDialog = exitDialog.get();
		exitDialog->show();
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

//void GameManager::setDisplayMode(DXGI_MODE_DESC displayMode) {
//	gameEngine->setDisplayMode(displayMode);
//}

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
