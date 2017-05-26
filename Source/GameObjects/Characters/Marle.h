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
	virtual void loadWeapon(shared_ptr<AssetSet> weaponSet,
		Vector3 weaponPositions[4]) override;
	vector<unique_ptr<Projectile>> projectiles;

	virtual void startMainAttack() override;
	virtual void attackUpdate(double deltaTime) override;

	USHORT nextBullet = 0;


	shared_ptr<Animation> shootLeft;
	shared_ptr<Animation> shootDown;
	shared_ptr<Animation> shootRight;
	shared_ptr<Animation> shootUp;
};