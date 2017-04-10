#include "../pch.h"
#include "GameManager.h"

Keyboard::KeyboardStateTracker keyTracker;
//GamePad::State gamePad0;

GameManager::GameManager(GameEngine* gmngn) {

	gameEngine = gmngn;
}

GameManager::~GameManager() {
}

#include "../Engine/GameEngine.h"
bool GameManager::initializeGame(HWND hwnd, ComPtr<ID3D11Device> dvc, shared_ptr<MouseController> ms,
	vector<shared_ptr<Joystick>> joysticks) {

	device = dvc;
	mouse = ms;

	{
		exitDialog.reset(guiFactory->createDialog(true, true));
		Vector2 dialogPos, dialogSize;
		dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
		dialogPos = dialogSize;
		dialogPos.x -= dialogSize.x / 2;
		dialogPos.y -= dialogSize.y / 2;
		exitDialog->setDimensions(dialogPos, dialogSize);
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

	//menuScreen.reset(new MenuManager());
	//menuScreen->setGameManager(this);
	//if (!menuScreen->initialize(device, mouse))
	//	return false;

	mapParser = make_unique<MapParser>();

	levelScreen = make_unique<LevelScreen>(joysticks);
	levelScreen->setGameManager(this);
	if (!levelScreen->initialize(device, mouse))
		return false;


	currentScreen = levelScreen.get();
	mouse->loadMouseIcon(guiFactory.get(), "Mouse Icon");
	ShowCursor(false);

	return true;
}


void GameManager::update(double deltaTime, shared_ptr<MouseController> mouse) {

	auto state = Keyboard::Get().GetState();
	keyTracker.Update(state);
	//gamePad0 = gamePad->GetState(0);
	currentScreen->update(deltaTime, mouse);
	if (exitDialog->isOpen)
		exitDialog->update(deltaTime);
}


void GameManager::draw(SpriteBatch * batch) {

	currentScreen->draw(batch);


	exitDialog->draw(batch);

}


void GameManager::startGame() {

	if (!levelScreen->initialize(device, mouse)) {
		GameEngine::showErrorDialog(L"Failed to load Level Screen", L"Gaah");
		return;
	}
	currentScreen = levelScreen.get();
}


bool GameManager::loadLevel(const wchar_t* file) {

	mapDoc.reset(new pugi::xml_document());
	if (!mapDoc->load_file(file)) {
		wostringstream wss;
		wss << "Error trying to read " << file << ".";
		GameEngine::showErrorDialog(wss.str(), L"Error reading map file");
		OutputDebugString(wss.str().c_str());
		return false;
	}

}


void GameManager::loadMainMenu() {

	mouse->loadMouseIcon(guiFactory.get(), "Mouse Icon");
	mouse->setRotation(0);

	lastScreen = currentScreen;
	currentScreen = levelScreen.get();

}


void GameManager::pause() {

	if (currentScreen != NULL)
		currentScreen->pause();
}

void GameManager::confirmExit() {
	if (!exitDialog->isOpen)
		exitDialog->open();
	else
		exitDialog->close();
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
