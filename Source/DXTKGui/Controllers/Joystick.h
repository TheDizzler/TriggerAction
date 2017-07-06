#pragma once
#include <Windows.h>
#include <SimpleMath.h>
//#include "../BaseGraphics/Sprite.h"
//#include "../StringHelper.h"

#include <hidsdi.h>
#include <Xinput.h>

#define MAX_BUTTONS		128

class PlayerCharacter;

enum PlayerSlotNumber {
	NONE = -1, SLOT_1, SLOT_2, SLOT_3
};

/* Virtual Sockets for controllers. */
enum ControllerSocketNumber {
	SOCKET_1, SOCKET_2, SOCKET_3
};

enum ControlButtons {
	A = 0, B, X, Y, L, R, SELECT, START
};

/* A joystick class with huge thanks to Alexander Bocken
	https://www.codeproject.com/Articles/185522/Using-the-Raw-Input-API-to-Process-Joystick-Input */
class Joystick {
public:

	Joystick(ControllerSocketNumber controllerSocket);
	~Joystick();

	virtual SHORT getXInputSlot() = 0;

	HANDLE getHandle();
	void registerNewHandle(HANDLE handle);

	virtual void parseRawInput(PRAWINPUT pRawInput) = 0;

	/* The virtual controller socket this joystick is plugged in to. */
	ControllerSocketNumber socket;
	
	void setPlayerSlotNumber(PlayerSlotNumber slotNum);
	PlayerSlotNumber getPlayerSlotNumber();

	/** Returns normalized direction magnitude (between -1 and 1). **/
	virtual DirectX::SimpleMath::Vector2 getDirection() = 0;
	virtual bool isLeftPressed() = 0;
	virtual bool isRightPressed() = 0;
	virtual bool isUpPressed() = 0;
	virtual bool isDownPressed() = 0;

	/** _ButtonPushed() is for one time check only. */
	virtual bool yButtonPushed() = 0;
	virtual bool xButtonPushed() = 0;
	virtual bool aButtonPushed() = 0;
	virtual bool bButtonPushed() = 0;
	virtual bool lButtonPushed() = 0;
	virtual bool rButtonPushed() = 0;
	virtual bool startButtonPushed() = 0;
	virtual bool selectButtonPushed() = 0;

	virtual bool yButtonDown() = 0;
	virtual bool xButtonDown() = 0;
	virtual bool aButtonDown() = 0;
	virtual bool bButtonDown() = 0;
	virtual bool lButtonDown() = 0;
	virtual bool rButtonDown() = 0;
	virtual bool startButtonDown() = 0;
	virtual bool selectButtonDown() = 0;

	ControlButtons startButton = ControlButtons::START;
	ControlButtons selectButton = ControlButtons::SELECT;
	ControlButtons attackButton = ControlButtons::Y;
	ControlButtons jumpButton = ControlButtons::X;
	ControlButtons runButton = ControlButtons::B;
	ControlButtons blockButton = ControlButtons::L;


protected:
	HANDLE handle = NULL;
	PlayerSlotNumber playerSlotNumber = PlayerSlotNumber::NONE;
};

class RawInputJoystick : public Joystick {
public:
	RawInputJoystick(ControllerSocketNumber controllerSocket);
	virtual ~RawInputJoystick();

	virtual SHORT getXInputSlot() override;

	virtual void parseRawInput(PRAWINPUT pRawInput) override;

	/** Returns normalized direction magnitude (between -1 and 1). **/
	virtual DirectX::SimpleMath::Vector2 getDirection() override;
	virtual bool isLeftPressed() override;
	virtual bool isRightPressed() override;
	virtual bool isUpPressed() override;
	virtual bool isDownPressed() override;

	virtual bool yButtonPushed() override;
	virtual bool xButtonPushed() override;
	virtual bool aButtonPushed() override;
	virtual bool bButtonPushed() override;
	virtual bool lButtonPushed() override;
	virtual bool rButtonPushed() override;
	virtual bool startButtonPushed() override;
	virtual bool selectButtonPushed() override;

	virtual bool yButtonDown() override;
	virtual bool xButtonDown() override;
	virtual bool aButtonDown() override;
	virtual bool bButtonDown() override;
	virtual bool lButtonDown() override;
	virtual bool rButtonDown() override;
	virtual bool startButtonDown() override;
	virtual bool selectButtonDown() override;


private:
	BOOL bButtonStates[MAX_BUTTONS];
	BOOL lastButtonStates[MAX_BUTTONS];
	LONG lAxisX = 0;
	LONG lAxisY = 0;
	LONG lAxisZ = 0;
	LONG lAxisRz = 0;
	LONG lHat = 0;
	INT  g_NumberOfButtons;

	USHORT DEAD_ZONE = 10;

};



class GamePadJoystick : public Joystick {
public:
	GamePadJoystick(ControllerSocketNumber controllerSocket, USHORT xInputNum);
	virtual ~GamePadJoystick();

	virtual SHORT getXInputSlot() override;

	void update();
	/** DO NOTHING */
	virtual void parseRawInput(PRAWINPUT pRawInput) override;

	/** Returns normalized direction magnitude (between -1 and 1). **/
	virtual DirectX::SimpleMath::Vector2 getDirection() override;

	virtual bool isLeftPressed() override;
	virtual bool isRightPressed() override;
	virtual bool isUpPressed() override;
	virtual bool isDownPressed() override;


	virtual bool yButtonPushed() override;
	virtual bool xButtonPushed() override;
	virtual bool aButtonPushed() override;
	virtual bool bButtonPushed() override;
	virtual bool lButtonPushed() override;
	virtual bool rButtonPushed() override;
	virtual bool startButtonPushed() override;
	virtual bool selectButtonPushed() override;


	virtual bool yButtonDown() override;
	virtual bool xButtonDown() override;
	virtual bool aButtonDown() override;
	virtual bool bButtonDown() override;
	virtual bool lButtonDown() override;
	virtual bool rButtonDown() override;
	virtual bool startButtonDown() override;
	virtual bool selectButtonDown() override;

private:
	USHORT xInputNum = 0;
	XINPUT_STATE state;
	XINPUT_STATE lastState;
};
