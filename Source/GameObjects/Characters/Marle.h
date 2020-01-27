#pragma once
#include "PlayerCharacter.h"

class Marle : public PlayerCharacter {
public:
	Marle(shared_ptr<PlayerSlot> slot);
	virtual ~Marle();

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;
private:
	virtual void initializeAssets() override;
	virtual void loadWeapon(AssetSet* weaponSet, Vector3 weaponPositions[4]) override;
	vector<unique_ptr<Projectile>> projectiles;

	virtual void startMainAttack() override;
	virtual void attackUpdate(double deltaTime) override;

	USHORT nextBullet = 0;

	Animation* shootLeft;
	Animation* shootDown;
	Animation* shootRight;
	Animation* shootUp;
};