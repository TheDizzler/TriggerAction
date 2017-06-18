#pragma once
#include "../DXTKGui/Effects/LetterJammer.h"


class JammerManager {
public:
	JammerManager();
	virtual ~JammerManager();

	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	void createJam(Vector3 position, wstring message, Color color = Color(1, 1, 1, 1));
	void createJam(Vector3 position, int number, Color color = Color(1, 1, 1, 1));

	void reset();
private:

	vector<unique_ptr<LetterJammer>> jammers;
};