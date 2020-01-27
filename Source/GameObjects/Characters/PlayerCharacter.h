#pragma once

#include "../../Managers/GFXAssetManager.h"
#include "../../Engine/PlayerSlot.h"
#include "../Projectile.h"
#include "../Creature.h"
#include "CharacterData.h"

const static USHORT MAX_PROJECTILES = 3;


class PlayerCharacter : public Creature {
public:
	PlayerCharacter(shared_ptr<PlayerSlot> slot);
	virtual ~PlayerCharacter();

	void reloadData(CharacterData* data);

	void setInitialPosition(const Vector3& startingPosition);

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch) override;

	virtual void takeDamage(int damage, bool showDamage = true) override;
	
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
protected:
	virtual void initializeAssets();

	shared_ptr<PlayerSlot> playerSlot;
	Joystick* joystick;
	CharacterData* characterData;	

	/** Possible actions. */
	void movement(double deltaTime);
	Vector3 getMovement(double deltaTime, Vector2 direction);
	void startDrawWeapon();
	virtual void startMainAttack() = 0;
	void startBlock();
	void startJump();

	/** Objects already hit by current attack. */
	vector<Tangible*> hitList;

	bool running = false;
	bool moving = false;

	const double NORMAL_SPEED = 1.1;
	const double RUN_SPEED = 1.75;
	const double JUMP_TIME = .375;
	const int MAX_JUMP_HEIGHT = 20;
	
	float moveRightSpeed = 75;
	float moveDownSpeed = moveRightSpeed * 2 / 3;
	/* Precalculate diagonal radius (at 45 degree) since it will always be the same
		(speed and direction don't change).
		Finding radius r at given angle Q in an oval with sub-axii a and b:
			r = (a*b) / sqrt( pow(a, 2) * pow(sin(Q), 2) + pow (b, 2) * pow (cos(Q), 2)) */
	float Q = XM_PIDIV4;
	float radius = (moveRightSpeed * moveDownSpeed)
		/ sqrt(pow(moveRightSpeed, 2) * pow(sin(Q), 2)
			+ pow(moveDownSpeed, 2) * pow(cos(Q), 2));
	float moveDiagonalRight = radius * cos(Q);
	float moveDiagonalDown = radius * sin(Q);
	float runRightSpeed = 100;

	bool lastCollision = false;
	void drawWeaponUpdate(double deltaTime);
	virtual void waitUpdate(double deltaTime);
	virtual void blockUpdate(double deltaTime);
	virtual void jumpUpdate(double deltaTime);
	virtual void hitUpdate(double deltaTime);
	
	Vector3 collisionMovement(Vector3& moveVector);

	virtual void loadWeapon(AssetSet* weaponSet, Vector3 weaponPositions[4]) = 0;

	Animation* runDown;
	Animation* runLeft;
	Animation* runUp;
	Animation* runRight;

	Animation* standDown;
	Animation* standLeft;
	Animation* standUp;
	Animation* standRight;

	Animation* jumpDown;
	Animation* jumpLeft;
	Animation* jumpUp;
	Animation* jumpRight;

	Animation* blockDown;
	Animation* blockLeft;
	Animation* blockUp;
	Animation* blockRight;

	Animation* combatStanceDown;
	Animation* combatStanceLeft;
	Animation* combatStanceUp;
	Animation* combatStanceRight;

	Animation* hitDown;
	Animation* hitLeft;
	Animation* hitUp;
	Animation* hitRight;
};