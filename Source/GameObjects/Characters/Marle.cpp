#include "../../pch.h"
#include "Marle.h"

Marle::Marle(shared_ptr<PlayerSlot> slot) : PlayerCharacter(slot) {
	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	HP = 65;
	MP = 12;
	PWR = 2;
	HIT = 10;
	MAG = 8;
	SPD = 8;
	EV = 8;
	STAM = 6;
	MDEF = 8;

	// Aura (9TP) -> Aura Whirl (Chrono)
	// Provoke
	// Ice -> Ice Sword (Chrono) -> Antipode (Lucca)
	// Cure
	// Haste
	// Ice 2
	// Cure 2
	// Life 2
}

Marle::~Marle() {
}

void Marle::loadWeapon(shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {

	projectiles.clear();

	for (int i = 0; i < MAX_PROJECTILES; ++i) {
		unique_ptr<Projectile> proj = make_unique<Projectile>(this, weaponPositions);
		proj->loadBullet(weaponSet->getAnimation("Bronze Bow Left"),
			weaponSet->getAsset("Bullet Shadow"));
		proj->loadHitEffect(weaponSet->getAnimation("Bronze Bow HitEffect"));
		projectiles.push_back(move(proj));
	}
}

void Marle::startMainAttack() {
	int horzDirection = joystick->lAxisX;
	int vertDirection = joystick->lAxisY;

	if (horzDirection > 10) {
		facing = Facing::RIGHT;
	} else if (horzDirection < -10) {
		facing = Facing::LEFT;
	} else if (vertDirection < -10) {
		facing = Facing::UP;
	} else if (vertDirection > 10) {
		facing = Facing::DOWN;
	}

	switch (facing) {
		case Facing::LEFT:
			loadAnimation("shoot left");
			break;
		case Facing::DOWN:
			loadAnimation("shoot down");
			break;
		case Facing::RIGHT:
			loadAnimation("shoot right");
			break;
		case Facing::UP:
			loadAnimation("shoot up");
			break;
	}
	currentFrameIndex = -1;
	currentFrameTime = currentFrameDuration;
	action = CreatureAction::ATTACKING_ACTION;
	moving = false;
	canCancelAction = false;
	attackUpdate(0);
}

void Marle::attackUpdate(double deltaTime) {
	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {

		switch (++currentFrameIndex) {
			case 0: // readying attack
				break;
			case 1: // firing bullet 
				projectiles[nextBullet++]->fire(facing, position);
				if (nextBullet >= MAX_PROJECTILES)
					nextBullet = 0;
				break;
			case 2: // recoil/cooldown
				break;
			case 3:
			default:
				// fully completed animation
				switch (facing) {
					case Facing::RIGHT:
						loadAnimation("combat stance right");
						break;
					case Facing::LEFT:
						loadAnimation("combat stance left");
						break;
					case Facing::DOWN:
						loadAnimation("combat stance down");
						break;
					case Facing::UP:
						loadAnimation("combat stance up");
						break;
				}
				canCancelAction = true;
				action = CreatureAction::WAITING_ACTION;
				return;

		}
		currentFrameTime = 0;
		currentFrameDuration
			= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
		currentFrameRect
			= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
		currentFrameOrigin
			= currentAnimation->animationFrames[currentFrameIndex]->origin;
	}
}
