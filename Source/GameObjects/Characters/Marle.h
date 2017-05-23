#pragma once
#include "PlayerCharacter.h"

class Marle : public PlayerCharacter {
public:
	Marle(shared_ptr<PlayerSlot> slot);
	virtual ~Marle();


private:
	virtual void loadWeapon(shared_ptr<AssetSet> weaponSet,
		Vector3 weaponPositions[4]) override;
	vector<unique_ptr<Projectile>> projectiles;

	virtual void startMainAttack() override;
	virtual void attackUpdate(double deltaTime) override;

	USHORT nextBullet = 0;

};