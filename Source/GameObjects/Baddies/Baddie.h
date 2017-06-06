#pragma once

#include "../Creature.h"

class BaddieData {
public:
	BaddieData();
	virtual ~BaddieData();


	void loadData(xml_node baddieDataNode, shared_ptr<AssetSet> assetSet);

	string type;
	shared_ptr<AssetSet> assets;
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


	virtual void takeDamage(int damage);


	USHORT GOLD = 0;
protected:

	Creature* target = NULL;
	virtual void startMainAttack(Vector3 direction) = 0;
	virtual void attackUpdate(double deltaTime) = 0;

	const double TIME_TO_DIE = 1.5;
	Color startTint;

	Vector3 weaponPositions[4];

protected: // creature unique stuff

	/** Range at which baddie will take an opportunity action. */
	Vector3 threatRange;
	float jumpSpeed;
	Vector3 jumpVelocity;
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
	bool drawAttack = false;
	unique_ptr<RectangleFrame> attackFrame;
};