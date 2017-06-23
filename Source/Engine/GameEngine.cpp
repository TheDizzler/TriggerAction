#include "../pch.h"
#include "GameEngine.h"

unique_ptr<GUIFactory> guiFactory;
unique_ptr<GFXAssetManager> gfxAssets;
bool gameInitialized = false;


unique_ptr<PromptDialog> GameEngine::errorDialog;
unique_ptr<PromptDialog> GameEngine::warningDialog;
Dialog* GameEngine::showDialog = NULL;
bool GameEngine::paused = false;
bool GameEngine::dialogCustom = false;

GameEngine::GameEngine() {
}


GameEngine::~GameEngine() {

	game.reset();
	if (audioEngine != NULL)
		audioEngine->Suspend();
	delete blendState;
	mouse.reset();
	errorDialog.reset();
	warningDialog.reset();
	gfxAssets.reset();
	guiFactory.reset();
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
		GameEngine::errorMessage(L"Stage Initialization Failed");
		return false;
	}

	paused = false;
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
	if (!game->initializeGame(hwnd, device, mouse)) {
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
	errorDialog.reset(guiFactory->createDialog(dialogSize, dialogPos, false, true, 5));


	//errorDialog->setDimensions(dialogPos, dialogSize);
	errorDialog->setTint(Color(0, 120, 207));
	unique_ptr<Button> quitButton;
	quitButton.reset(guiFactory->createButton());
	quitButton->setText(L"Exit Program");
	quitButton->setActionListener(new QuitButtonListener(this));
	//quitButton->setMatrixFunction([&]()-> Matrix { return camera->translationMatrix(); });
	errorDialog->setCancelButton(move(quitButton));

	ScrollBarDesc scrollBarDesc;
	scrollBarDesc.upButtonImage = "ScrollBar Up Custom";
	scrollBarDesc.upPressedButtonImage = "ScrollBar Up Pressed Custom";
	scrollBarDesc.trackImage = "ScrollBar Track Custom";
	scrollBarDesc.scrubberImage = "Scrubber Custom";
	warningDialog.reset(guiFactory->createDialog(dialogPos, dialogSize, false, true, 3));

	//warningDialog->setDimensions(dialogPos, dialogSize);
	warningDialog->setScrollBar(scrollBarDesc);
	//warningDialog->setMatrixFunction([&]()-> Matrix { return camera->translationMatrix(); });
	warningDialog->setTint(Color(0, 120, 207));
	warningDialog->setCancelButton(L"Continue");
	unique_ptr<Button> quitButton2;
	quitButton2.reset(guiFactory->createButton());
	quitButton2->setText(L"Exit Program");
	quitButton2->setActionListener(new QuitButtonListener(this));
	warningDialog->setConfirmButton(move(quitButton2));

	showDialog = warningDialog.get();

	mouse->loadMouseIcon(guiFactory.get(), "Mouse Arrow");
	mouse->hide();
	blendState = new CommonStates(device.Get());
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


void GameEngine::update(double deltaTime) {



	if (paused) {
		auto state = Keyboard::Get().GetState();
		keyTracker.Update(state);

		if (showDialog->isOpen()) {
			showDialog->update(deltaTime);
			if (dialogCustom) {
				MenuDialog* dialog = (MenuDialog*) showDialog;
				if (dialog->selectionMade) {
					switch (dialog->getSelected()) {
						case PauseMenuItems::QUIT:
							dialog->hide();
							paused = false;
							dialogCustom = false;
							exit();
							return;
						case PauseMenuItems::CONTINUE:
							dialog->hide();
							paused = false;
							dialogCustom = false;
							break;
						case PauseMenuItems::RELOAD_XML:
							dialog->hide();
							paused = false;
							dialogCustom = false;
							gfxAssets->initialize(device);
							game->reloadLevel(Globals::testLevel);
							break;

					}
				}
			} else {
				mouse->saveMouseState();
				if (keyTracker.IsKeyPressed(Keyboard::Escape)) {
					showDialog->hide();
					paused = false;
					mouse->hide();
					return;
				}
			}

		} else if (keyTracker.IsKeyPressed(Keyboard::Escape))
			game->confirmExit();
	} else
		game->update(deltaTime);

	guiOverlay->update(deltaTime);
}


#include "CommonStates.h"
void GameEngine::render() {

	deviceContext->ClearRenderTargetView(renderTargetView.Get(), Colors::GhostWhite);
	batch->Begin(SpriteSortMode_FrontToBack, blendState->NonPremultiplied(),
		NULL, NULL, NULL, NULL, camera->translationMatrix());
	{
		game->draw(batch.get());
	}
	batch->End();

	batch->Begin(SpriteSortMode_Deferred, blendState->NonPremultiplied());
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
		if (!paused)
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


void GameEngine::controllerRemoved(ControllerSocketNumber controllerSocket,
	PlayerSlotNumber slotNumber) {

	game->controllerRemoved(controllerSocket, slotNumber);
}

void GameEngine::newController(shared_ptr<Joystick> newStick) {

	game->newController(newStick);
}