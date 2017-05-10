#pragma once

#define MAX_PLAYERS 3


namespace Globals {

	extern int WINDOW_WIDTH;	// in pixels
	extern int WINDOW_HEIGHT;	// in pixels

	const static Vector2 targetResolution = /*Vector2(256, 224);*/ // true SNES resolution
		Vector2(512, 448);

	/**	0 = Present as fast as possible.
		1 = Lock to screen refresh rate. */
	extern int vsync_enabled;
	extern bool FULL_SCREEN;

	const static char* mapManifestFile = "assets/MapManifest.xml";

	/* For reference purposes. */
	/*wostringstream ws;
	ws << "some writing here" << someVariable << "\n";
	OutputDebugString(ws.str().c_str());*/
};
