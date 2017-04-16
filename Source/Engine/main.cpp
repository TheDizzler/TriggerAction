#include "../pch.h"
#pragma once

#include <guiddef.h>
//DEFINE_GUID(GUID_HID, 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30);
//#define GUID_CLASS_INPUT GUID_HID

#include <Mmsystem.h>
#include <Setupapi.h>


#include "GameEngine.h"


LPCTSTR wndClassName = L"Trigger Action";
HWND hwnd;

unique_ptr<GameEngine> gameEngine;
HDEVNOTIFY newInterface = NULL;

double countsPerSecond = 0.0;
__int64 counterStart = 0;

int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime = 0;

int registerControllers();
int getInputDeviceInfo(bool writeToFile, wstring filename = L"USB Devices.txt");
int messageLoop();
void startTimer();
double getTime();
double getFrameTime();

bool initWindow(HINSTANCE hInstance, int showWnd);
LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void updateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam);

GUID guidHid;


void releaseResources() {

	if (Globals::FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	if (newInterface)
		UnregisterDeviceNotification(newInterface);

	CoUninitialize();
}


/** Main windows function.
@nShowWnd how window should be displayed. Examples: SW_SHOWMAXIMIZED, SW_SHOW, SW_SHOWMINIMIZED. */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	//DEFINE_GUID(GUID_HID, 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30);
	//#define GUID_CLASS_INPUT GUID_HID
	guidHid = GUID();
	guidHid.Data1 = 0x4D1E55B2L;
	guidHid.Data2 = 0xF16F;
	guidHid.Data3 = 0x11CF;
	guidHid.Data4[0] = 0x88;
	guidHid.Data4[1] = 0xCB;
	guidHid.Data4[2] = 0x00;
	guidHid.Data4[3] = 0x11;
	guidHid.Data4[4] = 0x11;
	guidHid.Data4[5] = 0x00;
	guidHid.Data4[6] = 0x00;
	guidHid.Data4[7] = 0x30;

	gameEngine.reset(new GameEngine());

	if (!initWindow(hInstance, nShowCmd)) {
		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);
		releaseResources();
		return 0;
	}

	if (!gameEngine->initEngine(hwnd, hInstance)) {
		GameEngine::errorMessage(L"Game Engine Initialization Failed", L"Error", true);
		releaseResources();
		return 0;
	}

	// listen for new devices
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	//NotificationFilter.dbcc_classguid = KSCATEGORY_AUDIO;
	newInterface = RegisterDeviceNotification(hwnd, &NotificationFilter,
		DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
	//if (dev_notify == NULL) {
		//OutputDebugString(L"Could not register for devicenotifications!");
	//}


	messageLoop(); /* Main program loop */
	releaseResources();


	return 0;
}


int messageLoop() {

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { // if it's a windows message
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {	// game code
			frameCount++;
			if (getTime() > 1.0f) {
				fps = frameCount;
				frameCount = 0;
				startTimer();
			}

			double frameTime = getFrameTime();


			gameEngine->run(frameTime, fps);

		}

	}

	return msg.wParam;

}

int Globals::WINDOW_WIDTH = 800;
int Globals::WINDOW_HEIGHT = 600;
int Globals::vsync_enabled = 1;
bool Globals::FULL_SCREEN = false;
// SNES resolution 512x448 max (256x224 normally)

bool initWindow(HINSTANCE hInstance, int showWnd) {


	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;			// mo' styles http://msdn.microsoft.com/en-us/library/ms633574(VS.85).aspx#class_styles
	wc.lpfnWndProc = wndProc;
	wc.cbClsExtra = NULL;						// extra bytes after wc structure
	wc.cbWndExtra = NULL;						// extra bytes after windows instance
	wc.hInstance = hInstance;					// instance of current app
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);		// title bar icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	// mo' mouse cursors http://msdn.microsoft.com/en-us/library/ms648391(VS.85).aspx
	wc.hbrBackground = 0/*(HBRUSH) (COLOR_WINDOW)*/;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = wndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// taskbar icon

	if (!RegisterClassEx(&wc)) {

		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	int posX, posY;
	posX = posY = 0;
	int windowX, windowY;
	windowX = windowY = 0;

	//if (Globals::FULL_SCREEN) {

	//	// Determine the resolution of the clients desktop screen.
	//	windowX = GetSystemMetrics(SM_CXSCREEN);
	//	windowY = GetSystemMetrics(SM_CYSCREEN);
	//	Globals::WINDOW_WIDTH = windowX;
	//	Globals::WINDOW_HEIGHT = windowY;

	//	DEVMODE dmScreenSettings;
	//	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
	//	dmScreenSettings.dmSize = sizeof(dmScreenSettings);
	//	dmScreenSettings.dmPelsWidth = (unsigned long) Globals::WINDOW_WIDTH;
	//	dmScreenSettings.dmPelsHeight = (unsigned long) Globals::WINDOW_HEIGHT;
	//	dmScreenSettings.dmBitsPerPel = 32;
	//	dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	//	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	//	
	//	// Set the position of the window to the top left corner.

	//} else {
	// Make client dimensions WINDOW_WIDTHxWINDOW_HEIGHT
	// (as opposed to window dimensions)
	RECT winRect = {0, 0, Globals::WINDOW_WIDTH, Globals::WINDOW_HEIGHT};
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	windowX = winRect.right - winRect.left;
	windowY = winRect.bottom - winRect.top;
	// If windowed then set it to global default resolution
	// and place the window in the middle of the screen.
	posX = (GetSystemMetrics(SM_CXSCREEN) - Globals::WINDOW_WIDTH) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - Globals::WINDOW_HEIGHT) / 2;

	//}

	DWORD windowStyle = (WS_OVERLAPPEDWINDOW&~WS_THICKFRAME | /*WS_CAPTION | WS_SYSMENU | */WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	hwnd = CreateWindowEx(
		NULL,					// extended style, check em out here https://msdn.microsoft.com/en-us/library/61fe4bte(v=vs.140).aspx
		wndClassName,
		wndClassName,			// title bar text
		windowStyle,			// window style, mo' styles http://msdn.microsoft.com/zh-cn/library/czada357.aspx
		posX, posY,				// top left of window
		windowX, windowY,
		NULL,					// handle to parent window
		NULL,					// handle to a menu
		hInstance,				// instance of current program
		NULL);					// used for MDI client window

	if (!hwnd) {

		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, showWnd);
	UpdateWindow(hwnd);


	return true;

}




LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {


	switch (msg) {
		case WM_ACTIVATEAPP:
			Mouse::ProcessMessage(msg, wParam, lParam);
			Keyboard::ProcessMessage(msg, wParam, lParam);
			return 0;

		case WM_CREATE:

			//getInputDeviceInfo(true, L"Devices.txt");

			RAWINPUTDEVICE rid[1];
			rid[0].usUsagePage = 1;
			rid[0].usUsage = 4; // joystick
			rid[0].dwFlags = 0;
			rid[0].hwndTarget = hwnd;

			if (!RegisterRawInputDevices(rid, 1, sizeof(RAWINPUTDEVICE))) {
				OutputDebugString(L"On no!! Can't find Raw Input Device??");
				return -1;
			}


			registerControllers();

			return 0;

		case WM_INPUT:
		{
			PRAWINPUT pRawInput;
			UINT bufferSize;
			HANDLE hHeap;

			GetRawInputData((HRAWINPUT) lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
			hHeap = GetProcessHeap();
			pRawInput = (PRAWINPUT) HeapAlloc(hHeap, 0, bufferSize);
			if (!pRawInput)
				return 0;

			GetRawInputData((HRAWINPUT) lParam, RID_INPUT,
				pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));
			gameEngine->parseRawInput(pRawInput);

			HeapFree(hHeap, 0, pRawInput);

		}
		return 0;
		/*case MIM_DATA:
		case 	MIM_ERROR:
		case 	MIM_LONGDATA:
		case 	MIM_LONGERROR:
		case 	MIM_MOREDATA:
		case 	MIM_OPEN:
		case 	MM_MIM_CLOSE:
		case 	MM_MOM_CLOSE:
		case 	MM_MOM_DONE:
		case 	MM_MOM_OPEN:
		case 	MM_MOM_POSITIONCB:
			OutputDebugString(L"Ayy lmao!");
			return 0;*/
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			Mouse::ProcessMessage(msg, wParam, lParam);
			return 0;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Keyboard::ProcessMessage(msg, wParam, lParam);
			return 0;


		case WM_DEVICECHANGE:
		{

			PDEV_BROADCAST_HDR pDev = reinterpret_cast<PDEV_BROADCAST_HDR>(lParam);

			if (!pDev)
				break;


			if (pDev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
				PDEV_BROADCAST_DEVICEINTERFACE deviceInterface
					= reinterpret_cast<const PDEV_BROADCAST_DEVICEINTERFACE>(pDev);
				switch (wParam) {
					case DBT_DEVICEARRIVAL:
					{


						if (deviceInterface->dbcc_classguid == KSCATEGORY_AUDIO) {
							//OutputDebugString(L"Audio interface added!\n");
							if (gameEngine)
								gameEngine->onAudioDeviceChange();
							return 0;
						}


						if (deviceInterface->dbcc_classguid == guidHid) {
							registerControllers();
								//OutputDebugString(L"Found a controller!\n");
							return 0;
						}
					}
					break;

					case DBT_DEVICEREMOVECOMPLETE:

						if (deviceInterface->dbcc_classguid == KSCATEGORY_AUDIO) {

							OutputDebugString(L"Audio interface removed!\n");
							return 0;
						}
						if (deviceInterface->dbcc_classguid == guidHid) {
						// remove joysticks
							OutputDebugString(L"Joystick removed\n");
							//gameEngine->controllerRemoved(deviceInterface);
							registerControllers();
							gameEngine->controllerRemoved();
							return 0;
						}
						break;

				}

			}
		}
		return 0;

		case WM_NCLBUTTONDOWN:
			gameEngine->suspend();
			break;
		case WM_KILLFOCUS:
			gameEngine->suspend();
			//OutputDebugString(L"Lost Focus\n");
			return 0;

		case WM_ACTIVATE:
			//OutputDebugString(L"Got Focus\n");
			gameEngine->resume();
			return 0;

		case WM_DESTROY:	// top right x button pressed
			PostQuitMessage(0);
			return 0;

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//#include <afx.h>
//void updateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam) {
//	// dbcc_name:
//	// \\?\USB#Vid_04e8&Pid_503b#0002F9A9828E0F06#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
//	// convert to
//	// USB\Vid_04e8&Pid_503b\0002F9A9828E0F06
//	ASSERT(lstrlen(pDevInf->dbcc_name) > 4);
//	CString szDevId = pDevInf->dbcc_name + 4;
//	int idx = szDevId.ReverseFind(_T('#'));
//	ASSERT(-1 != idx);
//	szDevId.Truncate(idx);
//	szDevId.Replace(_T('#'), _T('\\'));
//	szDevId.MakeUpper();
//
//	CString szClass;
//	idx = szDevId.Find(_T('\\'));
//	ASSERT(-1 != idx);
//	szClass = szDevId.Left(idx);
//
//	// if we are adding device, we only need present devices
//	// otherwise, we need all devices
//	DWORD dwFlag = DBT_DEVICEARRIVAL != wParam
//		? DIGCF_ALLCLASSES : (DIGCF_ALLCLASSES | DIGCF_PRESENT);
//	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, szClass, NULL, dwFlag);
//	if (hDevInfo == INVALID_HANDLE_VALUE) {
//		//AfxMessageBox(CString("SetupDiGetClassDevs(): ")
//			//+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
//		OutputDebugString(L"SetupDiGetClassDevs() failed");
//		return;
//	}
//	
//
//	SP_DEVINFO_DATA* pspDevInfoData =
//		(SP_DEVINFO_DATA*) HeapAlloc(GetProcessHeap(), 0, sizeof(SP_DEVINFO_DATA));
//	pspDevInfoData->cbSize = sizeof(SP_DEVINFO_DATA);
//	for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, pspDevInfoData); i++) {
//		DWORD DataT;
//		DWORD nSize = 0;
//		TCHAR buf[MAX_PATH];
//
//		if (!SetupDiGetDeviceInstanceId(hDevInfo, pspDevInfoData, buf, sizeof(buf), &nSize)) {
//			//AfxMessageBox(CString("SetupDiGetDeviceInstanceId(): ")
//				//+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
//			OutputDebugString(L"SetupDiGetDeviceInstanceId() failed");
//			break;
//		}
//
//		if (szDevId == buf) {
//			// device found
//			if (SetupDiGetDeviceRegistryProperty(hDevInfo, pspDevInfoData,
//				SPDRP_FRIENDLYNAME, &DataT, (PBYTE) buf, sizeof(buf), &nSize)) {
//				// do nothing
//			} else if (SetupDiGetDeviceRegistryProperty(hDevInfo, pspDevInfoData,
//				SPDRP_DEVICEDESC, &DataT, (PBYTE) buf, sizeof(buf), &nSize)) {
//				// do nothing
//			} else {
//				lstrcpy(buf, _T("Unknown"));
//			}
//			// update UI
//			// .....
//			// .....
//			break;
//		}
//	}
//
//
//	if (pspDevInfoData)
//		HeapFree(GetProcessHeap(), 0, pspDevInfoData);
//	SetupDiDestroyDeviceInfoList(hDevInfo);
//}


int registerControllers() {

	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
		OutputDebugString(L"On no!! Can't find Raw Input Device??");
		return -1;
	}

	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST) malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) {
		OutputDebugString(L"Initialization failed...");
		return -1;
	}

	int nNoOfDevices = 0;
	if ((nNoOfDevices = GetRawInputDeviceList(
		pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST))) == ((UINT) -1)) {
		OutputDebugString(L"No devices found");
		return -1;
	}

	RID_DEVICE_INFO rdi;
	rdi.cbSize = sizeof(RID_DEVICE_INFO);
	//RAWINPUTDEVICE* rid;
	int numControllersFound = 0;

	//vector<int> controllerIndices;
	vector<HANDLE> controllerRawDevices;

	for (int i = 0; i < nNoOfDevices; i++) {
		UINT size = 256;
		TCHAR tBuffer[256] = {0};

		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, tBuffer, &size) < 0) {
			// Error in reading device name
			continue;
		}

		UINT cbSize = rdi.cbSize;
		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize) < 0) {
			// Error in reading information
			wostringstream wss;
			wss << L"Device Name: " << tBuffer << "\n";
			wss << "Error reading information" << endl;
			OutputDebugString(wss.str().c_str());
			continue;
		}


		if (rdi.dwType == RIM_TYPEHID) {

			if (rdi.hid.usUsage == 4 && rdi.hid.usUsagePage == 1) {
				controllerRawDevices.push_back(pRawInputDeviceList[i].hDevice);
					/*++numControllersFound;
					HANDLE handle = pRawInputDeviceList[i].hDevice;
					gameEngine->addJoystick(handle);*/

			}

		}


	}

	gameEngine->addJoysticks(controllerRawDevices);


	free(pRawInputDeviceList);
	return 1;
}

#include <fstream>

/* Finds and list all HID devices. For device finding, debugging, etc. */
int getInputDeviceInfo(bool writeToFile, wstring filename) {



	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
		OutputDebugString(L"On no!! Can't find Raw Input Device??");
		return -1;
	}

	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST) malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) {
		OutputDebugString(L"Initialization failed...");
		return -1;
	}

	int nNoOfDevices = 0;
	if ((nNoOfDevices = GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST))) == ((UINT) -1)) {
		OutputDebugString(L"No devices found");
		return -1;
	}

	RID_DEVICE_INFO rdi;
	rdi.cbSize = sizeof(RID_DEVICE_INFO);
	wostringstream wss;
	wss << "Number of devices: " << nNoOfDevices << endl << "***********************" << endl;
	for (int i = 0; i < nNoOfDevices; i++) {
		UINT size = 256;
		TCHAR tBuffer[256] = {0};

		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, tBuffer, &size) < 0) {
			// Error in reading device name
			wss << "Error reading device name" << endl;
			//continue;
		}


		wss << L"Device Name: " << tBuffer << "\n";
		UINT cbSize = rdi.cbSize;
		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize) < 0) {
			// Error in reading information
			wss << "Error reading information" << endl;
		}

		if (rdi.dwType == RIM_TYPEMOUSE) {
			wss << "ID for Mouse:" << rdi.mouse.dwId << endl;
			wss << "Number of Buttons:" << rdi.mouse.dwNumberOfButtons << endl;
			wss << "Sample rate(Number of data points):" << rdi.mouse.dwSampleRate << endl;
		} else if (rdi.dwType == RIM_TYPEKEYBOARD) {
			wss << "Keyboard Mode:" << rdi.keyboard.dwKeyboardMode << endl;
			wss << "Number of function keys:" << rdi.keyboard.dwNumberOfFunctionKeys << endl;
			wss << "Number of indicators:" << rdi.keyboard.dwNumberOfIndicators << endl;
			wss << "Number of keys total: " << rdi.keyboard.dwNumberOfKeysTotal << endl;
			wss << "Type of the keyboard: " << rdi.keyboard.dwType << endl;
			wss << "Subtype of the keyboard: " << rdi.keyboard.dwSubType << endl;
		} else if (rdi.dwType == RIM_TYPEHID) {
			wss << "Vendor Id:" << rdi.hid.dwVendorId << endl;
			wss << "Product Id:" << rdi.hid.dwProductId << endl;
			wss << "Version No:" << rdi.hid.dwVersionNumber << endl;
			wss << "Usage for the device: " << rdi.hid.usUsage << endl;
			wss << "Usage Page for the device: " << rdi.hid.usUsagePage << endl;


		}
		wss << "***********************" << endl << endl;
	}

	if (writeToFile) {
		wofstream file;
		file = wofstream(filename);
		file << wss.str();
		file.close();

	} else
		OutputDebugString(wss.str().c_str());

	free(pRawInputDeviceList);
	return 0;
}


void startTimer() {

	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	counterStart = frequencyCount.QuadPart;

}


double getTime() {

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - counterStart) / countsPerSecond;
}


double getFrameTime() {

	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / countsPerSecond;
}