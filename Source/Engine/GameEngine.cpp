#include "../pch.h"
#include "GameEngine.h"

unique_ptr<GUIFactory> guiFactory;
unique_ptr<GFXAssetManager> gfxAssets;



unique_ptr<PromptDialog> GameEngine::errorDialog;
unique_ptr<PromptDialog> GameEngine::warningDialog;
Dialog* GameEngine::showDialog = NULL;



GameEngine::GameEngine() {
}


GameEngine::~GameEngine() {

	if (audioEngine != NULL)
		audioEngine->Suspend();
}


bool GameEngine::initEngine(HWND hw, HINSTANCE hInstance) {

	hwnd = hw;

	if (!initD3D(hwnd)) {
		GameEngine::errorMessage(L"Direct3D Initialization Failed", L"Error");
		return false;
	}

	if (!initRawInput(hwnd)) {
		GameEngine::errorMessage(L"Raw Input Init failed", L"Error");
		return false;
	}

	//joyListener = make_unique<ControllerListener>(this);

	// Initialize Audio Engine
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	AUDIO_ENGINE_FLAGS audioFlags = AudioEngine_Default;
//#ifdef _DEBUG
//	audioFlags = audioFlags | AudioEngine_Debug;
//#endif
	audioEngine = make_unique<AudioEngine>(audioFlags);
	retryAudio = false;

	if (!audioEngine->IsAudioDevicePresent()) {
		// no audio device found. Operating in silent mode.

	}

	if (!initGFXAssets()) {
		GameEngine::errorMessage(L"GFX Assets Initialization Failed");

		return false;
	}


	if (!initStage()) {
		//MessageBox(0, L"Stage Initialization Failed", L"Error", MB_OK);
		GameEngine::errorMessage(L"Stage Initialization Failed");
		return false;
	}

	gameInitialized = true;
	return true;
}

void GameEngine::onAudioDeviceChange() {
	retryAudio = true;
}


#include "../DXTKGui/GuiAssets.h"
bool GameEngine::initGFXAssets() {

	// get graphical assets from xml file
	docAssMan = make_unique<pugi::xml_document>();
	if (!docAssMan->load_file(GUIAssets::assetManifestFile)) {
		GameEngine::errorMessage(L"Could not read AssetManifest file!",
			L"Fatal Read Error!");
		return false;
	}

	xml_node guiAssetsNode = docAssMan->child("root").child("gui");
	guiFactory = make_unique<GUIFactory>(hwnd, guiAssetsNode);
	if (!guiFactory->initialize(device, deviceContext,
		swapChain, batch.get(), mouse)) {

		//MessageBox(0, L"Failed to load GUIFactory", L"Fatal Error", MB_OK);
		GameEngine::errorMessage(L"Failed to load GUIFactory", L"Fatal Error");
		return false;
	}

	initErrorDialogs();


	gfxAssets = make_unique<GFXAssetManager>(docAssMan->child("root"));
	if (!gfxAssets->initialize(device)) {
		showErrorDialog(L"Failed to load GFXAssets Manager", L"Fatal Error");
		return false;
	}

	return true;
}

bool GameEngine::initStage() {

	game = make_unique<GameManager>(this);
	if (!game->initializeGame(hwnd, device, mouse/*, joysticks*/)) {
		GameEngine::errorMessage(L"Game Manager failed to load.", L"Critical Failure");
		return false;
	}
	return true;
}


void GameEngine::initErrorDialogs() {

	Vector2 dialogPos, dialogSize;
	dialogSize = Vector2(Globals::WINDOW_WIDTH / 2, Globals::WINDOW_HEIGHT / 2);
	dialogPos = dialogSize;
	dialogPos.x -= dialogSize.x / 2;
	dialogPos.y -= dialogSize.y / 2;
	errorDialog = guiFactory->createDialog(dialogSize, dialogPos, false, true, 5);


	//errorDialog->setDimensions(dialogPos, dialogSize);
	errorDialog->setTint(Color(0, 120, 207));
	unique_ptr<Button> quitButton;
	quitButton.reset(guiFactory->createButton());
	quitButton->setText(L"Exit Program");
	quitButton->setOnClickListener(new QuitButtonListener(this));
	//quitButton->setMatrixFunction([&]()-> Matrix { return camera->translationMatrix(); });
	errorDialog->setCancelButton(move(quitButton));

	ScrollBarDesc scrollBarDesc;
	scrollBarDesc.upButtonImage = "ScrollBar Up Custom";
	scrollBarDesc.upPressedButtonImage = "ScrollBar Up Pressed Custom";
	scrollBarDesc.trackImage = "ScrollBar Track Custom";
	scrollBarDesc.scrubberImage = "Scrubber Custom";
	warningDialog = guiFactory->createDialog(dialogPos, dialogSize, false, true, 3);

	//warningDialog->setDimensions(dialogPos, dialogSize);
	warningDialog->setScrollBar(scrollBarDesc);
	//warningDialog->setMatrixFunction([&]()-> Matrix { return camera->translationMatrix(); });
	warningDialog->setTint(Color(0, 120, 207));
	warningDialog->setCancelButton(L"Continue");
	unique_ptr<Button> quitButton2;
	quitButton2.reset(guiFactory->createButton());
	quitButton2->setText(L"Exit Program");
	quitButton2->setOnClickListener(new QuitButtonListener(this));
	warningDialog->setConfirmButton(move(quitButton2));

	showDialog = warningDialog.get();
}

bool warningCanceled = false;
void GameEngine::run(double deltaTime) {


	update(deltaTime);
	render();
	if (!audioEngine->IsAudioDevicePresent() && !warningCanceled) {
		// no audio device found. Operating in silent mode.
		showWarningDialog(L"No audio device found. Operating in Silent Mode.\nEnd Message...",
			L"Audio Engine failure");
		warningCanceled = true;
	}

	if (retryAudio) {
		retryAudio = false;
		if (audioEngine->Reset()) {
			// restart looped sounds
		}
	} else if (!audioEngine->Update()) {
		if (audioEngine->IsCriticalError()) {
			//ErrorDialog(L"Audio device lost!", L"Audio Engine failure");
			retryAudio = true;
		}
	}
}


void GameEngine::controllerRemoved(size_t controllerSlot) {

	game->controllerRemoved(controllerSlot/*lostDevices*/);
}

void GameEngine::newController(HANDLE joyHandle) {

	game->newController(joyHandle);
}


void GameEngine::update(double deltaTime) {

	mouse->saveMouseState();

	if (paused) {
		auto state = Keyboard::Get().GetState();
		keyTracker.Update(state);

		if (showDialog->isOpen()) {

			if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
				showDialog->hide();
				paused = false;
				return;
			}
			showDialog->update(deltaTime);
		} else if (keyTracker.IsKeyPressed(Keyboard::Escape))
			game->confirmExit();
	} else
		game->update(deltaTime, mouse);

	guiOverlay->update(deltaTime, mouse);
}


#include "CommonStates.h"
void GameEngine::render() {

	deviceContext->ClearRenderTargetView(renderTargetView.Get(), Colors::GhostWhite);
	CommonStates blendState(device.Get());
	batch->Begin(SpriteSortMode_FrontToBack/*, NULL, NULL, NULL, NULL, NULL, camera->translationMatrix()*/);
	{
		game->draw(batch.get());
	}
	batch->End();

	batch->Begin(SpriteSortMode_Deferred, blendState.NonPremultiplied());
	{
		guiOverlay->draw(batch.get());
		showDialog->draw(batch.get());
		mouse->draw(batch.get());
	}
	batch->End();


	swapChain->Present(0, 0);
}

void GameEngine::suspend() {

	stopFullScreen();
	if (game != NULL) {
		game->pause();
	}
	if (audioEngine != NULL)
		audioEngine->Suspend();
}

void GameEngine::resume() {

	setFullScreen(Globals::FULL_SCREEN);
	if (audioEngine != NULL)
		audioEngine->Resume();
}

void GameEngine::exit() {
	if (swapChain.Get() != NULL)
		swapChain->SetFullscreenState(false, NULL);
	DestroyWindow(hwnd);
}


