#include "../../pch.h"
#include "Marle.h"

Marle::Marle(shared_ptr<PlayerSlot> slot) : PlayerCharacter(slot) {
	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	initializeAssets();

	loadAnimation(standRight);
	currentFrameTexture = currentAnimation->texture.Get();

	currentHP = maxHP = 65;
	currentMP = maxMP = 12;
	PWR = 2;
	HIT = 10;
	MAG = 8;
	SPD = 8;
	EV = 8;
	STAM = 6;
	MDEF = 8;

	weight = 60;

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

void Marle::update(double deltaTime) {
	PlayerCharacter::update(deltaTime);
	for (const auto& bullet : projectiles)
		bullet->update(deltaTime);
}

void Marle::draw(SpriteBatch* batch) {
	PlayerCharacter::draw(batch);
	for (const auto& bullet : projectiles)
		bullet->draw(batch);
}

void Marle::initializeAssets() {
	PlayerCharacter::initializeAssets();

	shootLeft = assetSet->getAnimation("shoot left");
	shootDown = assetSet->getAnimation("shoot down");
	shootRight = assetSet->getAnimation("shoot right");
	shootUp = assetSet->getAnimation("shoot up");

	jumpLeft = assetSet->getAnimation("walk left");
	jumpDown = assetSet->getAnimation("walk down");
	jumpRight = assetSet->getAnimation("walk right");
	jumpUp = assetSet->getAnimation("walk up");
}

void Marle::loadWeapon(shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {

	projectiles.clear();

	for (int i = 0; i < MAX_PROJECTILES; ++i) {
		unique_ptr<BronzeBow> proj = make_unique<BronzeBow>
			(this, weaponSet, weaponPositions);
		projectiles.push_back(move(proj));
	}
}

void Marle::startMainAttack() {

	moveVelocity.x = 0;
	moveVelocity.y = 0;

	/*int horzDirection = joystick->lAxisX;
	int vertDirection = joystick->lAxisY;

	if (horzDirection > 10) {
		facing = Facing::RIGHT;
	} else if (horzDirection < -10) {
		facing = Facing::LEFT;
	} else if (vertDirection < -10) {
		facing = Facing::UP;
	} else if (vertDirection > 10) {
		facing = Facing::DOWN;
	}*/

	if (joystick->isRightPressed()) {
		facing = Facing::RIGHT;
	} else if (joystick->isLeftPressed()) {
		facing = Facing::LEFT;
	} else if (joystick->isUpPressed()) {
		facing = Facing::UP;
	} else if (joystick->isDownPressed()) {
		facing = Facing::DOWN;
	}

	switch (facing) {
		case Facing::LEFT:
			loadAnimation(shootLeft);
			break;
		case Facing::DOWN:
			loadAnimation(shootDown);
			break;
		case Facing::RIGHT:
			loadAnimation(shootRight);
			break;
		case Facing::UP:
			loadAnimation(shootUp);
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
			case 1: // firing bullet // recoil/cooldown
				projectiles[nextBullet++]->fire(facing, position);
				if (nextBullet >= MAX_PROJECTILES)
					nextBullet = 0;
				break;
			case 2:
				canCancelAction = true;
				break;
			case 3:
			default:
				// fully completed animation
				switch (facing) {
					case Facing::RIGHT:
						loadAnimation(combatStanceRight);
						break;
					case Facing::LEFT:
						loadAnimation(combatStanceLeft);
						break;
					case Facing::DOWN:
						loadAnimation(combatStanceDown);
						break;
					case Facing::UP:
						loadAnimation(combatStanceUp);
						break;
				}
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
