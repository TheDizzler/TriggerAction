#pragma once

#include "../Managers/GFXAssetManager.h"
#include "../Engine/Joystick.h"


static enum Facing {
	RIGHT, DOWN, LEFT, UP
};

class PlayerCharacter {
public:
	PlayerCharacter(shared_ptr<Joystick> joystick);
	~PlayerCharacter();

	virtual void initialize(const shared_ptr<AssetSet> characterAssetSet, int startingPosition);

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch);

private:

	shared_ptr<Joystick> joystick;

	shared_ptr<AssetSet> assetSet;
	shared_ptr<Animation> currentAnimation;

	bool getMovement(double deltaTime, int horzDirection, int vertDirection);
	void loadAnimation(const pugi::char_t* name);

	int facing = RIGHT;

	double currentFrameTime = -1;
	int currentFrameIndex = 0;

	Vector2 position;
	Vector2 origin;
	Color tint = DirectX::Colors::White.v;
	float rotation = 0.0f;
	float layerDepth = 0.0f;
	Vector2 scale = Vector2(1, 1);
	SpriteEffects spriteEffects = SpriteEffects_None;

	bool moving = false;
	bool waiting = false;
	float moveRightSpeed = 50;
	float moveDownSpeed = moveRightSpeed * 2 / 3;
	/* Precalculate diagonal radius (at 45 degree) since it will always be the same
		(speed and direction don't change).
		Finding radius r at given angle Q in an oval with sub-axii a and b:
			r = (a*b) / sqrt( pow(a, 2) * pow(sin(Q), 2) + pow (b, 2) * pow (cos(Q), 2))
	*/
	float Q = XM_PIDIV4;
	float radius = (moveRightSpeed*moveDownSpeed)
		/ sqrt(pow(moveRightSpeed, 2) * pow(sin(Q), 2) + pow(moveDownSpeed, 2) * pow(cos(Q), 2));
	float moveDiagonalRight = radius * cos(Q);
	float moveDiagonalDown = radius * sin(Q);
	float runRightSpeed = 100;


};