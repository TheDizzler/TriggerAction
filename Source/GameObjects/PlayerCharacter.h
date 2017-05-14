#pragma once

#include "../Managers/GFXAssetManager.h"
#include "../Managers/PlayerSlot.h"
#include "Creature.h"




class PlayerCharacter : public Creature {
public:
	PlayerCharacter(shared_ptr<PlayerSlot> slot);
	virtual ~PlayerCharacter();

	void setInitialPosition(const Vector2& startingPosition);

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch);

	

	string name;
private:

	shared_ptr<PlayerSlot> playerSlot;
	Joystick* joystick;
	CharacterData* characterData;
	

	bool getMovement(double deltaTime, int horzDirection, int vertDirection);
	void startMainAttack();


	bool running = false;
	bool moving = false;
	bool waiting = false;
	float moveRightSpeed = 75;
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


	bool lastHit = false;

};