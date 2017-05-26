#include "../../pch.h"
#include "Lucca.h"

Lucca::Lucca(shared_ptr<PlayerSlot> slot) : PlayerCharacter(slot) {
	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	initializeAssets();

	loadAnimation(standRight);
	currentFrameTexture = currentAnimation->texture.Get();

	//currentHP = maxHP = 
	//currentMP = maxMP = 
	//PWR = ;
	//HIT = ;
	//MAG = ;
	//SPD = ;
	//EV = ;
	//STAM = ;
	//MDEF = ;

	// Flame Toss -> Fire Whirl (Chrono)
	// Hypno Wave
	// Fire - > Antipode (Marle) -> Fire Sword(Chrono)
	// Napalm
	// Protect
	// Fire 2
	// Mega Bomb
	// Flare

}

Lucca::~Lucca() {
}

void Lucca::update(double deltaTime) {

	PlayerCharacter::update(deltaTime);
	for (const auto& bullet : projectiles)
		bullet->update(deltaTime);
}

void Lucca::draw(SpriteBatch * batch) {
	PlayerCharacter::draw(batch);
	for (const auto& bullet : projectiles)
		bullet->draw(batch);
}

void Lucca::initializeAssets() {
	PlayerCharacter::initializeAssets();

	shootLeft = assetSet->getAnimation("shoot left");
	shootDown = assetSet->getAnimation("shoot down");
	shootRight = assetSet->getAnimation("shoot right");
	shootUp = assetSet->getAnimation("shoot up");
}

void Lucca::loadWeapon(shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {

	projectiles.clear();

	for (int i = 0; i < MAX_PROJECTILES; ++i) {
		unique_ptr<AirGun> proj = make_unique<AirGun>
			(this, weaponSet, weaponPositions);
		projectiles.push_back(move(proj));
	}
}


void Lucca::startMainAttack() {
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


const int ANIMATION_REPEATS = 1;
void Lucca::attackUpdate(double deltaTime) {

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {

		switch (++currentFrameIndex) {
			case 0: // readying attack
				animationRepeats = 0;
				fired = false;
				break;
			case 1: // firing bullet 
				if (fired)
					break;
				fired = true;
				projectiles[nextBullet++]->fire(facing, position);
				if (nextBullet >= MAX_PROJECTILES)
					nextBullet = 0;
				break;
			case 2: // recoil/cooldown
				break;
			case 3: // after animations (cancelable from here)
				if (animationRepeats++ < ANIMATION_REPEATS) {
					currentFrameIndex = 1;
					return;
				}

				canCancelAction = true;
				break;
			case 4:
				break;
			case 5:
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
