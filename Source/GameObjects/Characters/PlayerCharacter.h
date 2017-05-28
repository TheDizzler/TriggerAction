#pragma once

#include "../../Managers/GFXAssetManager.h"
#include "../../Managers/PlayerSlot.h"
#include "../Projectile.h"
#include "../Creature.h"
#include "CharacterData.h"

const static USHORT MAX_PROJECTILES = 3;


class PlayerCharacter : public Creature {
public:
	PlayerCharacter(shared_ptr<PlayerSlot> slot);
	virtual ~PlayerCharacter();

	void reloadData(CharacterData* data);

	void setInitialPosition(const Vector2& startingPosition);

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch);


	string name;
	USHORT currentHP, maxHP;
	USHORT currentMP, maxMP;
	/** Damage from physical attacks. */
	USHORT PWR;
	/** Damage from ranged attacks. */
	USHORT HIT;
	/** Damage from magical attacks. */
	USHORT MAG;
	/** Move speed and cast times. */
	USHORT SPD;
	/** Defense against physical and ranged attacks. */
	USHORT EV;
	/** Damage reduction and HP. */
	USHORT STAM;
	/** Defense against magical attacks. */
	USHORT MDEF;

	/** Total attack power: PWR + WPN damage. */
	USHORT ATKPWR;
	/** Total defense power: EV + ARM. */
	USHORT DEFPWR;

	USHORT EXP = 0;
protected:
	virtual void initializeAssets();

	shared_ptr<PlayerSlot> playerSlot;
	Joystick* joystick;
	CharacterData* characterData;	

	Vector3 getMovement(double deltaTime, int horzDirection, int vertDirection);
	void startDrawWeapon();
	virtual void startMainAttack() = 0;
	void startBlock();
	void startJump();

	Vector3 startJumpPosition, endHalfJumpPosition;
	double jumpTime = 0;
	bool jumpingRising = true;
	bool running = false;
	bool moving = false;
	bool waiting = false;

	const double NORMAL_SPEED = 1.0;
	const double RUN_SPEED = 1.75;
	const double JUMP_TIME = .375;
	const int MAX_JUMP_HEIGHT = 20;
	//const int MAX_JUMP_DISTANCE = 30;
	float moveRightSpeed = 75;
	float moveDownSpeed = moveRightSpeed * 2 / 3;
	/* Precalculate diagonal radius (at 45 degree) since it will always be the same
		(speed and direction don't change).
		Finding radius r at given angle Q in an oval with sub-axii a and b:
			r = (a*b) / sqrt( pow(a, 2) * pow(sin(Q), 2) + pow (b, 2) * pow (cos(Q), 2))
	*/
	float Q = XM_PIDIV4;
	float radius = (moveRightSpeed * moveDownSpeed)
		/ sqrt(pow(moveRightSpeed, 2) * pow(sin(Q), 2)
			+ pow(moveDownSpeed, 2) * pow(cos(Q), 2));
	float moveDiagonalRight = radius * cos(Q);
	float moveDiagonalDown = radius * sin(Q);
	float runRightSpeed = 100;

	float jumpByX;
	float jumpByY;


	bool lastCollision = false;
	//virtual void attackUpdate(double deltaTime) override;
	void drawWeaponUpdate(double deltaTime);
	virtual void waitUpdate(double deltaTime);
	virtual void blockUpdate(double deltaTime);
	virtual void jumpUpdate(double deltaTime);

	/** Possible actions. */
	void movement(double deltaTime);
	/* A temp hitbox to test incoming collisions. */
	Hitbox radarBox;

	virtual void loadWeapon(shared_ptr<AssetSet> weaponSet,
		Vector3 weaponPositions[4]) = 0;


	shared_ptr<Animation> standDown;
	shared_ptr<Animation> standLeft;
	shared_ptr<Animation> standUp;
	shared_ptr<Animation> standRight;

	shared_ptr<Animation> jumpDown;
	shared_ptr<Animation> jumpLeft;
	shared_ptr<Animation> jumpUp;
	shared_ptr<Animation> jumpRight;

	shared_ptr<Animation> blockDown;
	shared_ptr<Animation> blockLeft;
	shared_ptr<Animation> blockUp;
	shared_ptr<Animation> blockRight;

	shared_ptr<Animation> combatStanceDown;
	shared_ptr<Animation> combatStanceLeft;
	shared_ptr<Animation> combatStanceUp;
	shared_ptr<Animation> combatStanceRight;
};