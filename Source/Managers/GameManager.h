#pragma once

#include <Keyboard.h>

#include "../Screens/TitleScreen.h"
#include "../DXTKGui/Controls/GUIFactory.h"
#include "../Screens/LevelScreen.h"
#include "../Screens/GUIOverlay.h"

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


	bool initializeGame(HWND hwnd, ComPtr<ID3D11Device> device, shared_ptr<MouseController> mouse/*,
		vector<shared_ptr<Joystick>> joysticks*/);


	void update(double deltaTime, shared_ptr<MouseController> mouse);
	void draw(SpriteBatch* batch);

	void startGame();
	bool loadLevel(const pugi::char_t* file);
	void loadMainMenu();

	void controllerRemoved(vector<shared_ptr<Joystick>> lostDevices);
	void newController(HANDLE joyHandle);
	void controllerAccepted(shared_ptr<Joystick> newJoy);

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


private:
	unique_ptr<xml_document> mapManifest;
	unique_ptr<xml_document> mapDoc;
	unique_ptr<MapParser> mapParser;

	string mapsDir;
	map<string, string> mapFiles;

	Screen* currentScreen = 0;
	Screen* lastScreen = 0;

	unique_ptr<TitleScreen> titleScreen;
	unique_ptr<LevelScreen> levelScreen;

	GameEngine* gameEngine;
	shared_ptr<MouseController> mouse;
	ComPtr<ID3D11Device> device;

	unique_ptr<Dialog> exitDialog;

};

class OnClickListenerDialogQuitButton : public Button::OnClickListener {
public:
	OnClickListenerDialogQuitButton(GameManager* screen) : main(screen) {
	}
	virtual void onClick(Button* button) override {
		main->exitDialog->close();
		main->exit();
	}
private:
	GameManager* main;
};