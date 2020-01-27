#pragma once
#include "PlayerCharacter.h"


class Lucca : public PlayerCharacter {
public:
	Lucca(shared_ptr<PlayerSlot> slot);
	virtual ~Lucca();

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;
private:
	virtual void initializeAssets() override;
	virtual void loadWeapon(AssetSet* weaponSet, Vector3 weaponPositions[4]) override;
	vector<unique_ptr<Projectile>> projectiles;

	virtual void startMainAttack() override;
	virtual void attackUpdate(double deltaTime) override;

	USHORT nextBullet = 0;
	bool fired = false;
	USHORT animationRepeats = 0;

	Animation* shootLeft;
	Animation* shootDown;
	Animation* shootRight;
	Animation* shootUp;
};