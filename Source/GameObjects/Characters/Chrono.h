#pragma once
#include "PlayerCharacter.h"

class Chrono : public PlayerCharacter {
public:
	Chrono(shared_ptr<PlayerSlot> slot);
	virtual ~Chrono();

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;
private:
	virtual void initializeAssets() override;
	virtual void loadWeapon(shared_ptr<AssetSet> weaponSet,
		Vector3 weaponPositions[4]) override;
	virtual void attackUpdate(double deltaTime) override;
	virtual void startMainAttack() override;

	Hitbox attackBox;
	Vector3 attackBoxSizes[4];
	Vector2 attackBoxOffset = Vector2(12, 12);
	bool drawAttack = false;
	unique_ptr<RectangleFrame> attackFrame;

};