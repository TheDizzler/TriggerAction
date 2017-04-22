#pragma once

#include "../GUIObjects/PCSelectDialog.h"
#include "../Engine/Joystick.h"

#include "../globals.h"



class PlayerSlot {
public:
	PlayerSlot(size_t slotNum) : slotNumber(slotNum) {
	}

	/** Returns true if pair was succesful. Returns false if slot already paired. */
	bool pairWithSocket(shared_ptr<Joystick> newJoy) {
		if (joystick != NULL) {
			//unpairSocket();
			OutputDebugString(L"Player Slot already connected\n");
			return false;
		}
		joystick = newJoy.get();
		joystick->playerSlot = slotNumber;

		wostringstream wss;
		wss << L"Connecting Socket " << joystick->socket << L" to Player Slot " << slotNumber << endl;
		OutputDebugString(wss.str().c_str());
		return true;
	}


	void unpairSocket() {
		wostringstream wss;
		wss << L"Socket " << joystick->socket << L" and Player Slot " << slotNumber;
		wss << L" unpaired." << endl;
		OutputDebugString(wss.str().c_str());

		joystick->playerSlot = -1;
		joystick = NULL;

	}


	void pairWithDialog(PCSelectDialog* dialog) {

		pcDialog = dialog;
		pcDialog->playerSlot = this;
	}

	Joystick* joystick = NULL;
	size_t slotNumber;

	PCSelectDialog* pcDialog;
};



class GUIOverlay {
public:
	shared_ptr<PlayerSlot>  playerSlots[MAX_PLAYERS];
public:
	GUIOverlay();
	~GUIOverlay();


	void update(double deltaTime, shared_ptr<MouseController> mouse);
	void draw(SpriteBatch* batch);

	void setDialogText(USHORT playerSlot, wstring text);

	void reportLostJoystick(size_t joystickSocket);
	void controllerRemoved(size_t playerSlot);
	//void unpairedJoystickRemoved(JoyData* joyData);
	void controllerTryingToPair(JoyData* joyData);
	//void pairPlayerSlotWith(JoyData* joyData);
	void finalizePair(JoyData* joyData);

	void readyPCSelect(shared_ptr<PlayerSlot> playerSlot);

private:

	int numPCsAvailable = 2;
	const char_t* pcs[2] = {"Marle", "Frog"};
	size_t nextAvaiablePC = 0;

	/** The order of these is important!! */
	enum HUDDIALOG {
		ENEMIES, PLAYERSTATS, PLAYER1, PLAYER2, PLAYER3
	};
	unique_ptr<PCSelectDialog> hudDialogs[HUDDIALOG::PLAYER3 + 1];

	unique_ptr<TextLabel> fpsLabel;

	CRITICAL_SECTION cs_waitingJoysticks;
	//vector<JoyData*> waitingForInput;
	vector<unique_ptr<Dialog>> lostJoyDialogs;

	vector<int> displayingLostJoys;

	/*enum WaitingForInputTast {
		READ_INPUT, REMOVE_JOYSTICK_FROM_WAITING
	};
	void sharedResource(size_t task, PVOID pvoid);*/

	unique_ptr<PCSelectDialog> createPCDialog(shared_ptr<AssetSet> dialogImageSet,
		const Vector2& position, const Vector2& size, const char_t* fontName = "Default Font");
};


class ControllerDialog : public PromptDialog {
public:
	ControllerDialog(GUIFactory* guiFactory);

	virtual void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 10);

	virtual void update(double deltaTime) override;


	virtual void setText(wstring text) override;

private:
	wstring defaultText;
	double dialogOpenTime = 0;
	const double CONTROLLER_WAIT_TIME = 1.0;
	int ellipsisii = 16;
	bool first = true;
};