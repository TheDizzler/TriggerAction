#pragma once


#include "GraphicsEngine.h"
#include "Input.h"

#include "../Managers/GameManager.h"
#include "../Managers/GFXAssetManager.h"


extern GUIFactory guiFactory;
extern unique_ptr<GFXAssetManager> gfxAssets;
extern bool gameInitialized;

/** The engine to connect higher level game code to the low level
	graphic and OS software. This class should be reusable for any 2D game,
	thus should not contain any game logic. */
class GameEngine : public GraphicsEngine, public Input {
public:

	GameEngine();
	virtual ~GameEngine();

	bool initEngine(HWND hwnd, HINSTANCE hInstance);
	void onAudioDeviceChange();

	virtual void reloadGraphicsAssets() override;

	void run(double time);


	static bool paused;
	static bool dialogCustom;
	void suspend();
	void resume();
	void exit();

	/** Returns true when HRESULT failed. */
	static inline bool reportError(HRESULT hr,
		wstring failMessage = L"This is SRS Error",
		wstring failTitle = L"Fatal Error",
		bool showMessageBox = false, bool dontUseGUI = false) {

		if (FAILED(hr)) {

			_com_error err(hr);
			wostringstream wss;
			wss << failMessage;
			wss << "\nHRESULT: " << err.ErrorMessage() << endl;
			if (GUIFactory::initialized && !dontUseGUI)
				GameEngine::showWarningDialog(wss.str(), failTitle);
			else if (!Globals::FULL_SCREEN && showMessageBox)
				MessageBox(NULL, wss.str().c_str(), failTitle.c_str(), MB_OK | MB_ICONERROR);

			failTitle += L" >> " + failMessage + L"\n\tHRESULT: " + err.ErrorMessage() + L"\n";
			OutputDebugString(failTitle.c_str()); // always output debug just in case
			return true;
		}

		return false;
	}


	static void errorMessage(wstring message, wstring title = L"Fatal Error",
		bool showMessageBox = false) {

		message += L"\n";
		if (!Globals::FULL_SCREEN && showMessageBox)
			MessageBox(NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);

		title += L" >> " + message;
		OutputDebugString(title.c_str()); // always output debug just in case
	}


	static void showErrorDialog(wstring message, wstring title, bool outputDebug = true) {
		dialogCustom = false;
		errorDialog->show();
		errorDialog->setTitle(title);
		errorDialog->setText(message);
		showDialog = errorDialog.get();
		if (outputDebug) {
			title += L" >> " + message = L"\n";
			OutputDebugString(title.c_str());
		}
	}

	static void showWarningDialog(wstring message, wstring title) {
		dialogCustom = false;
		warningDialog->show();
		warningDialog->setTitle(title);
		warningDialog->setText(message);
		warningDialog->setTextTint(Color(1, 0, 0, 1));
		showDialog = warningDialog.get();
	}

	static void showCustomDialog(Dialog* dialog) {
		paused = true;
		dialogCustom = true;
		showDialog = dialog;
		showDialog->show();
	}

	static Dialog* showDialog;

	virtual void controllerRemoved(ControllerSocketNumber controllerSocket,
		PlayerSlotNumber slotNumber) override;
	virtual void newController(shared_ptr<Joystick> newStick) override;


private:

	unique_ptr<AudioEngine> audioEngine;
	GameManager game;

	void update(double time);
	virtual void render() override;

	bool initGFXAssets();
	bool initStage();
	void initErrorDialogs();


	HWND hwnd;
	bool retryAudio;

	unique_ptr<pugi::xml_document> docAssMan;


	/* Critical error dialog. Exits game when dismissed. */
	static unique_ptr<PromptDialog> errorDialog;
	/* Minor error dialog. Choice between exit game and continue. */
	static unique_ptr<PromptDialog> warningDialog;

	CommonStates* blendState;

};

class QuitButtonListener : public Button::ActionListener {
public:
	QuitButtonListener(GameEngine* eng) : engine(eng) {
	}
	virtual void onClick(Button* button) override {
		engine->exit();
	}
	virtual void onPress(Button* button) override {
	};
	virtual void onHover(Button* button) override {
	};
	virtual void resetState(Button * button) override{};
	GameEngine* engine;

};
