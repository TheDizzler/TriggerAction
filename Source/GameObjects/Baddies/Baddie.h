#pragma once

#include "../Creature.h"

class BaddieData {
public:
	BaddieData();
	virtual ~BaddieData();

	void loadData(xml_node baddieDataNode, AssetSet* assetSet);

	string type;
	AssetSet* assets;
	unique_ptr<Hitbox> hitbox;
	Vector3 weaponPositions[4];
	Vector3 attackBoxSizes[4];
};


class Baddie : public Creature {
public:
	Baddie(BaddieData* baddieData);
	virtual ~Baddie();

	/* Returns true when creature should be removed from game. */
	virtual bool update(double deltaTime);
	virtual void draw(SpriteBatch* batch) override;

	virtual void takeDamage(int damage, bool showDamage = true);

	USHORT GOLD = 0;
protected:
	Creature* target = NULL;
	virtual void startMainAttack(Vector3 direction) = 0;
	virtual void attackUpdate(double deltaTime) = 0;

	void startJump(const Vector3& direction);
	void jumpUpdate(double deltaTime);

	float moveRightSpeed = 50;
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

	/** Objects already hit by current attack. */
	vector<Tangible*> hitList;

	Vector3 collisionMovement(Vector3 moveVector);

	const double TIME_TO_DIE = 1.5;
	Color startTint;

	Vector3 weaponPositions[4];
protected: // creature unique stuff
	/** Range at which baddie will take an opportunity action. */
	Vector3 threatRange;
	float jumpSpeed;
	float jumpHeight;
};



class BlueImp : public Baddie {
public:
	BlueImp(BaddieData* baddieData);
	virtual ~BlueImp();

	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;
protected:
	virtual void startMainAttack(Vector3 direction) override;
	virtual void attackUpdate(double deltaTime) override;

	Vector3 weaponPositions[4];

	Hitbox attackBox;
	Vector3 attackBoxSizes[4];
	Vector2 attackBoxOffset = Vector2(12, 12);
	bool drawAttackBox = false;
	unique_ptr<RectangleFrame> attackFrame;
};