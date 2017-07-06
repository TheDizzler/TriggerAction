#include "Joystick.h"
//#include "../Managers/GameManager.h"

double hz = 0.0;
__int64 startTime = 0;
__int64 lastFrameStart = 0;
int frames = 0;
void start() {

	LARGE_INTEGER frequencyCount;
	/*QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);*/

	QueryPerformanceCounter(&frequencyCount);
	startTime = frequencyCount.QuadPart;
	lastFrameStart = frequencyCount.QuadPart;
}

double getTimeSinceStart() {

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - startTime) / hz;
}



Joystick::Joystick(ControllerSocketNumber controllerSocket) {

	socket = controllerSocket;


}

Joystick::~Joystick() {
}

void Joystick::registerNewHandle(HANDLE hndl) {
	handle = hndl;

	/*LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);
	hz = double(frequencyCount.QuadPart);
	start();*/
}

void Joystick::setPlayerSlotNumber(PlayerSlotNumber slotNum) {
	playerSlotNumber = slotNum;
}

PlayerSlotNumber Joystick::getPlayerSlotNumber() {
	return playerSlotNumber;
}


HANDLE Joystick::getHandle() {
	return handle;
}


/** ***************** **/
RawInputJoystick::RawInputJoystick(ControllerSocketNumber controllerSocket)
	: Joystick(controllerSocket) {

	ZeroMemory(bButtonStates, sizeof(bButtonStates));
}

RawInputJoystick::~RawInputJoystick() {
}

SHORT RawInputJoystick::getXInputSlot() {
	return -1;
}


void RawInputJoystick::parseRawInput(PRAWINPUT pRawInput) {

	PHIDP_PREPARSED_DATA pPreparsedData = NULL;
	HIDP_CAPS            Caps;
	PHIDP_BUTTON_CAPS    pButtonCaps = NULL;
	PHIDP_VALUE_CAPS     pValueCaps = NULL;
	USHORT               capsLength;
	UINT                 bufferSize;
	HANDLE               hHeap;
	USAGE                usage[MAX_BUTTONS];
	ULONG                i, usageLength, value;

	memmove(lastButtonStates, bButtonStates, sizeof(bButtonStates));
	hHeap = GetProcessHeap();

	//
	// Get the preparsed data block
	//
	if (GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) != 0) {
		return;
	}
	if (!(pPreparsedData = (PHIDP_PREPARSED_DATA) HeapAlloc(hHeap, 0, bufferSize))) {

		return;
	}
	if (!((int) GetRawInputDeviceInfo(
		pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize) >= 0)) {
		HeapFree(hHeap, 0, pPreparsedData);
		pPreparsedData = NULL;
		return;
	}

	//
	// Get the joystick's capabilities
	//

	// Button caps
	if (HidP_GetCaps(pPreparsedData, &Caps) != HIDP_STATUS_SUCCESS) {
		HeapFree(hHeap, 0, pPreparsedData);
		pPreparsedData = NULL;
		return;
	}
	if (!(pButtonCaps = (PHIDP_BUTTON_CAPS) HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps))) {
		HeapFree(hHeap, 0, pPreparsedData);
		pPreparsedData = NULL;
		return;
	}

	capsLength = Caps.NumberInputButtonCaps;
	if (HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS) {
		HeapFree(hHeap, 0, pPreparsedData);
		HeapFree(hHeap, 0, pButtonCaps);
		pPreparsedData = NULL;
		pButtonCaps = NULL;
		return;
	}
	g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;

	// Value caps
	if (!(pValueCaps = (PHIDP_VALUE_CAPS) HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps))) {
		HeapFree(hHeap, 0, pPreparsedData);
		HeapFree(hHeap, 0, pButtonCaps);
		pPreparsedData = NULL;
		pButtonCaps = NULL;
		return;

	}
	capsLength = Caps.NumberInputValueCaps;
	if (HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS) {
		HeapFree(hHeap, 0, pPreparsedData);
		HeapFree(hHeap, 0, pButtonCaps);
		HeapFree(hHeap, 0, pValueCaps);
		pPreparsedData = NULL;
		pButtonCaps = NULL;
		pValueCaps = NULL;
		return;
	}

	//
	// Get the pressed buttons
	//
	usageLength = g_NumberOfButtons;
	if (HidP_GetUsages(HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
		(PCHAR) pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS) {

		HeapFree(hHeap, 0, pPreparsedData);
		HeapFree(hHeap, 0, pButtonCaps);
		HeapFree(hHeap, 0, pValueCaps);
		pPreparsedData = NULL;
		pButtonCaps = NULL;
		pValueCaps = NULL;
		return;
	}

	ZeroMemory(bButtonStates, sizeof(bButtonStates));

	for (i = 0; i < usageLength; i++)
		bButtonStates[usage[i] - pButtonCaps->Range.UsageMin] = TRUE;

	//
	// Get the state of discrete-valued-controls
	//

	for (i = 0; i < Caps.NumberInputValueCaps; i++) {
		if (HidP_GetUsageValue(HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin,
			&value, pPreparsedData, (PCHAR) pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid)
			!= HIDP_STATUS_SUCCESS) {
			HeapFree(hHeap, 0, pPreparsedData);
			HeapFree(hHeap, 0, pButtonCaps);
			HeapFree(hHeap, 0, pValueCaps);
			pPreparsedData = NULL;
			pButtonCaps = NULL;
			pValueCaps = NULL;
			return;
		}

		switch (pValueCaps[i].Range.UsageMin) {
			case 0x30:	// X-axis
				lAxisX = (LONG) value - 128;
				break;

			case 0x31:	// Y-axis
				lAxisY = (LONG) value - 128;
				break;

				//case 0x32: // Z-axis
				//	lAxisZ = (LONG) value - 128;
				//	break;

				//case 0x35: // Rotate-Z
				//	lAxisRz = (LONG) value - 128;
				//	break;

				//case 0x39:	// Hat Switch
				//	lHat = value;
				//	break;
		}
	}

	//wostringstream wss;
	//wss << "Buttons: " << g_NumberOfButtons << "\n";
	//wss << "bButtonStates[0]: " << bButtonStates[0] << "\n";
	//wss << "bButtonStates[1]: " << bButtonStates[1] << "\n";
	//wss << "Controller: " << socket << endl;
	//wss << lAxisX << " " << lAxisY << "\n";
	//OutputDebugString(wss.str().c_str());



	HeapFree(hHeap, 0, pPreparsedData);
	HeapFree(hHeap, 0, pButtonCaps);
	HeapFree(hHeap, 0, pValueCaps);
	//pPreparsedData = NULL;
	//pButtonCaps = NULL;
	//pValueCaps = NULL;

	/*double fpsUpdateTime = getTimeSinceStart();
	++frames;
	if (fpsUpdateTime > 1.0f) {

	wostringstream wss;
	wss << "Joystick: " << endl;
	wss << "frameCount: " << frames << " fpsUpdateTime: " << fpsUpdateTime << endl;
	wss << "fps: " << frames / fpsUpdateTime;
	guiOverlay->fps2Label->setText(wss);
	frames = 0;
	start();
	}*/
}

DirectX::SimpleMath::Vector2 RawInputJoystick::getDirection() {

	LONG x = 0, y = 0;
	if (lAxisX < -DEAD_ZONE || lAxisX > DEAD_ZONE)
		x = lAxisX;
	if (lAxisY < -DEAD_ZONE || lAxisY > DEAD_ZONE)
		y = lAxisY;
	auto dir = DirectX::SimpleMath::Vector2(x, y);
	dir.Normalize();
	return dir;
}

bool RawInputJoystick::isLeftPressed() {
	return lAxisX < -DEAD_ZONE;
}

bool RawInputJoystick::isRightPressed() {
	return lAxisX > DEAD_ZONE;
}

bool RawInputJoystick::isUpPressed() {
	return lAxisY < -DEAD_ZONE;
}

bool RawInputJoystick::isDownPressed() {
	return lAxisY > DEAD_ZONE;
}

bool RawInputJoystick::yButtonPushed() {
	bool pushed = bButtonStates[ControlButtons::Y]
		&& !lastButtonStates[ControlButtons::Y];

	if (pushed) {
		bButtonStates[ControlButtons::Y] = true;
		lastButtonStates[ControlButtons::Y] = true;
	}
	return pushed;
}

bool RawInputJoystick::xButtonPushed() {
	bool pushed = bButtonStates[ControlButtons::X]
		&& !lastButtonStates[ControlButtons::X];

	if (pushed) {
		bButtonStates[ControlButtons::X] = true;
		lastButtonStates[ControlButtons::X] = true;
	}
	return pushed;
}

bool RawInputJoystick::aButtonPushed() {
	bool pushed = bButtonStates[ControlButtons::A] && !lastButtonStates[ControlButtons::A];

	if (pushed) {
		bButtonStates[ControlButtons::A] = true;
		lastButtonStates[ControlButtons::A] = true;
	}
	return pushed;
}

bool RawInputJoystick::bButtonPushed() {
	bool pushed = bButtonStates[ControlButtons::B]
		&& !lastButtonStates[ControlButtons::B];

	if (pushed) {
		bButtonStates[ControlButtons::B] = true;
		lastButtonStates[ControlButtons::B] = true;
	}
	return pushed;
}

bool RawInputJoystick::lButtonPushed() {
	bool pushed = bButtonStates[ControlButtons::L]
		&& !lastButtonStates[ControlButtons::L];

	if (pushed) {
		bButtonStates[ControlButtons::L] = true;
		lastButtonStates[ControlButtons::L] = true;
	}
	return pushed;
}

bool RawInputJoystick::rButtonPushed() {
	bool pushed = bButtonStates[ControlButtons::R]
		&& !lastButtonStates[ControlButtons::R];

	if (pushed) {
		bButtonStates[ControlButtons::R] = true;
		lastButtonStates[ControlButtons::R] = true;
	}
	return pushed;
}

bool RawInputJoystick::startButtonPushed() {
	bool pushed = bButtonStates[ControlButtons::START]
		&& !lastButtonStates[ControlButtons::START];

	if (pushed) {
		bButtonStates[ControlButtons::START] = true;
		lastButtonStates[ControlButtons::START] = true;
	}
	return pushed;
}

bool RawInputJoystick::selectButtonPushed() {
	return bButtonStates[ControlButtons::SELECT] && !lastButtonStates[ControlButtons::SELECT];
}


bool RawInputJoystick::yButtonDown() {
	return bButtonStates[ControlButtons::Y];
}

bool RawInputJoystick::xButtonDown() {
	return bButtonStates[ControlButtons::X];
}

bool RawInputJoystick::aButtonDown() {
	return bButtonStates[ControlButtons::A];
}

bool RawInputJoystick::bButtonDown() {
	return bButtonStates[ControlButtons::B];
}

bool RawInputJoystick::lButtonDown() {
	return bButtonStates[ControlButtons::L];
}

bool RawInputJoystick::rButtonDown() {
	return bButtonStates[ControlButtons::R];
}

bool RawInputJoystick::startButtonDown() {
	return bButtonStates[startButton];
}

bool RawInputJoystick::selectButtonDown() {
	return bButtonStates[selectButton];
}




/** ******* GamePad ******* **/
GamePadJoystick::GamePadJoystick(ControllerSocketNumber controllerSocket, USHORT xINum)
	: Joystick(controllerSocket) {

	xInputNum = xINum;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
}

GamePadJoystick::~GamePadJoystick() {
}

SHORT GamePadJoystick::getXInputSlot() {
	return xInputNum;
}


void GamePadJoystick::update() {

	lastState = state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	/*DWORD dwResult = */XInputGetState(xInputNum, &state);

	//if (dwResult == ERROR_SUCCESS) {



}

/** DO NOTHING */
void GamePadJoystick::parseRawInput(PRAWINPUT pRawInput) {
}

DirectX::SimpleMath::Vector2 GamePadJoystick::getDirection() {

	SHORT lAxisX = state.Gamepad.sThumbLX;
	SHORT lAxisY = state.Gamepad.sThumbLY;
	float x = 0, y = 0;
	if (lAxisX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		|| lAxisX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		x = lAxisX / 32768.0;
	if (lAxisY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		|| lAxisY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		y = lAxisY / 32768.0;
	auto dir = DirectX::SimpleMath::Vector2(x, -y);
	//dir.Normalize();
	return dir;
}

bool GamePadJoystick::isLeftPressed() {
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0)
		return true;

	SHORT lAxisX = state.Gamepad.sThumbLX;

	if (lAxisX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return true;

	return false;
}

bool GamePadJoystick::isRightPressed() {
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0)
		return true;

	SHORT lAxisX = state.Gamepad.sThumbLX;

	if (lAxisX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return true;

	return false;
}

bool GamePadJoystick::isUpPressed() {
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0)
		return true;

	SHORT lAxisY = state.Gamepad.sThumbLY;

	if (lAxisY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return true;

	return false;
}

bool GamePadJoystick::isDownPressed() {
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0)
		return true;

	SHORT lAxisY = state.Gamepad.sThumbLY;

	if (lAxisY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return true;

	return false;
}

bool GamePadJoystick::yButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
}

bool GamePadJoystick::xButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_X) == 0;
}

bool GamePadJoystick::aButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_A) == 0;
}

bool GamePadJoystick::bButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_B) == 0;
}

bool GamePadJoystick::lButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == 0;
}

bool GamePadJoystick::rButtonPushed() {

	//float amount = state.Gamepad.bRightTrigger / 255;
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) == 0;
}

bool GamePadJoystick::startButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_START) == 0;
}

bool GamePadJoystick::selectButtonPushed() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0
		&& (lastState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) == 0;
}


bool GamePadJoystick::yButtonDown() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;;
}

bool GamePadJoystick::xButtonDown() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;;
}

bool GamePadJoystick::aButtonDown() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;;
}

bool GamePadJoystick::bButtonDown() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;;
}

bool GamePadJoystick::lButtonDown() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;;
}

bool GamePadJoystick::rButtonDown() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;;
}

bool GamePadJoystick::startButtonDown() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;;
}

bool GamePadJoystick::selectButtonDown() {
	return (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;;
}

