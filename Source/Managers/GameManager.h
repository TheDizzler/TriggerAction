#pragma once


#include <Keyboard.h>

#include "../Screens/TitleScreen.h"
#include "../DXTKGui/GUIFactory.h"
#include "../Screens/LevelScreen.h"
#include "../Screens/GUIOverlay.h"
#include "../Screens/OptionsScreen.h"
#include "../DXTKGui/Effects/ScreenTransitions.h"


class GameEngine;

extern Keyboard::KeyboardStateTracker keyTracker;
extern unique_ptr<GUIOverlay> guiOverlay;


/** The lowest level of class where game code should be included.
	Everything below this (GameEngine downward) should generally go unmodified. */
class GameManager {
	friend class OnClickListenerDialogQuitButton;
public:
	GameManager(GameEngine* gameEngine);
	~GameManager();


	bool initializeGame(HWND hwnd, ComPtr<ID3D11Device> device,
		shared_ptr<MouseController> mouse);


	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	void startGame();
	bool loadLevel(const pugi::char_t* levelName);
	bool reloadLevel(const pugi::char_t* levelName);

	void loadMainMenu();
	void loadOptionsScreen();

	void controllerRemoved(ControllerSocketNumber controllerSocket,
		PlayerSlotNumber slotNumber);
	void newController(shared_ptr<Joystick> newStick);
	void controllerAccepted(HANDLE handle);

	void setPaused(bool paused);
	void pause();
	void confirmExit();
	void exit();


	vector<ComPtr<IDXGIAdapter> > getAdapterList();
	vector<ComPtr<IDXGIOutput> > getDisplayList();
	vector<ComPtr<IDXGIOutput> > getDisplayListFor(size_t displayIndex);
	vector<ComPtr<IDXGIOutput> > getDisplayListFor(ComPtr<IDXGIAdapter> adapter);
	vector<DXGI_MODE_DESC> getDisplayModeList(size_t displayIndex);
	vector<DXGI_MODE_DESC> getDisplayModeList(ComPtr<IDXGIOutput> display);

	bool setAdapter(size_t adapterIndex);
	bool setDisplayMode(size_t displayModeIndex);
	bool setFullScreen(bool isFullScreen);

	size_t getSelectedAdapterIndex();
	size_t getSelectedDisplayIndex();
	size_t getSelectedDisplayModeIndex();



	unique_ptr<xml_document> mapManifest;
	unique_ptr<xml_document> mapDoc;
	unique_ptr<MapParser> mapParser;

	string mapsDir;
	map<string, string> mapFiles;
private:
	Screen* currentScreen = 0;
	Screen* lastScreen = 0;

	//unique_ptr<ScreenTransitions::ScreenTransitionManager> transMan;
	ScreenTransitions::ScreenTransitionManager transMan;

	void (GameManager::*updateFunction)(double) = NULL;
	void (GameManager::*drawFunction)(SpriteBatch*) = NULL;
	
	void normalUpdate(double deltaTime);
	void transitionUpdate(double deltaTime);

	void normalDraw(SpriteBatch* batch);
	void transitionDraw(SpriteBatch* batch);

	unique_ptr<TitleScreen> titleScreen;
	unique_ptr<LevelScreen> levelScreen;
	unique_ptr<OptionsScreen> optionsScreen;

	GameEngine* gameEngine;
	shared_ptr<MouseController> mouse;
	ComPtr<ID3D11Device> device;

	unique_ptr<PromptDialog> exitDialog;

};

class OnClickListenerDialogQuitButton : public Button::ActionListener {
public:
	OnClickListenerDialogQuitButton(GameManager* screen) : main(screen) {
	}
	virtual void onClick(Button* button) override {
		main->exitDialog->hide();
		main->exit();
	}
	virtual void onPress(Button* button) override {
	};
	virtual void onHover(Button* button) override {
	};
private:
	GameManager* main;
};

class CancelDialogButton : public Button::ActionListener {
public:
	CancelDialogButton(GameEngine* eng, Dialog* dlg) : engine(eng), dialog(dlg) {
	}
	virtual void onClick(Button* button) override;
	virtual void onPress(Button* button) override;
	virtual void onHover(Button* button) override;
private:
	GameEngine* engine;
	Dialog* dialog;
};