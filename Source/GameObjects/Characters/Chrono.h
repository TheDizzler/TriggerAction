#pragma once
#include "PlayerCharacter.h"

class Chrono : public PlayerCharacter {
public:
	Chrono(shared_ptr<PlayerSlot> slot);
	virtual ~Chrono();


private:
	virtual void loadWeapon(shared_ptr<AssetSet> weaponSet,
		Vector3 weaponPositions[4]) override;
	virtual void attackUpdate(double deltaTime) override;
	virtual void startMainAttack() override;

};