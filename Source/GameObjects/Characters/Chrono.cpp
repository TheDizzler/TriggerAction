#include "../../pch.h"
#include "Chrono.h"

#include "../../Engine/GameEngine.h"
Chrono::Chrono(shared_ptr<PlayerSlot> slot) : PlayerCharacter(slot) {
	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	initializeAssets();

	loadAnimation(standRight);
	currentFrameTexture = currentAnimation->texture.Get();

	currentHP = maxHP = 70;
	currentMP = maxMP = 8;
	PWR = 5;
	HIT = 8;
	MAG = 5;
	SPD = 13;
	EV = 8;
	STAM = 8;
	MDEF = 2;

	attackBoxSizes[Facing::LEFT] = Vector3(16, 16, 45);
	attackBoxSizes[Facing::UP] = Vector3(24, 16, 45);
	attackBoxSizes[Facing::RIGHT] = Vector3(16, 16, 45);
	attackBoxSizes[Facing::DOWN] = Vector3(24, 16, 45);

	attackBox.size = attackBoxSizes[facing];
#ifdef  DEBUG_HITBOXES
	attackFrame.reset(guiFactory->createRectangleFrame(
		Vector2(attackBox.position.x, attackBox.position.y),
		Vector2(attackBox.size.x, attackBox.size.y)));
#endif //  DEBUG_HITBOXES


	// Cyclone(4TP) -> Fire Whirl (Lucca) -> Aura Whirl (Marle)
	// Slash  -> Fire Sword (Lucca) -> Ice Sword (Marle)
	// Lightning
	// Spincut
	// Lightning 2
	// Life
	// Confuse
	// Luminaire
}

Chrono::~Chrono() {
}




void Chrono::update(double deltaTime) {
	PlayerCharacter::update(deltaTime);
#ifdef  DEBUG_HITBOXES
	attackFrame->update();
#endif //  DEBUG_HITBOXES
}

void Chrono::draw(SpriteBatch * batch) {
	PlayerCharacter::draw(batch);
#ifdef  DEBUG_HITBOXES
	if (drawAttack)
		attackFrame->draw(batch);
#endif //  DEBUG_HITBOXES
}

int repeatAttackCycles = 0;
void Chrono::attackUpdate(double deltaTime) {

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		switch (++currentFrameIndex) {
			case 1: // first attack
				if (repeatAttackCycles == 0) {
					attackBox.size = attackBoxSizes[facing];
					attackBox.position = position;
					switch (facing) {
						case Facing::LEFT:
							attackBox.position.x -= (currentFrameOrigin.x + attackBox.size.x);
							attackBox.position.y += 5;
							break;
						case Facing::DOWN:
							attackBox.position.y += attackBoxOffset.y;
							attackBox.position.x -= (getWidth() + currentFrameOrigin.x) / 2;
							break;
						case Facing::RIGHT:
							attackBox.position.x += (currentFrameOrigin.x);
							attackBox.position.y += 5;
							break;
						case Facing::UP:
							attackBox.position.x -= (getWidth() + currentFrameOrigin.x) / 2;
							break;
					}


					attackBox.position.y -= attackBox.size.y;

#ifdef  DEBUG_HITBOXES
					drawAttack = true;
					attackFrame->setSize(Vector2(attackBox.size.x, attackBox.size.y));
					attackFrame->setPosition(Vector2(
						attackBox.position.x, attackBox.position.y));
#endif //  DEBUG_HITBOXES

				// hit detection
					for (Tangible* object : hitboxesAll) {
						if (object == this) {
							continue;
						}

						if (attackBox.collisionZ(object->getHitbox())) {
							object->takeDamage(5);
							// slash effect

						}
					}

					break;
				} else {



					break;
				}
			case 2:

#ifdef  DEBUG_HITBOXES
				drawAttack = false;
#endif //  DEBUG_HITBOXES
				break;
			case 3:
				if (++repeatAttackCycles < 5) {
					currentFrameIndex = 1;
					currentFrameDuration = .2;
					break;
				}
				break;
			case 5:
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
				canCancelAction = true;
				return;
		}
		currentFrameTime = 0;

		currentFrameRect
			= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
		currentFrameOrigin
			= currentAnimation->animationFrames[currentFrameIndex]->origin;
	}
}

void Chrono::startMainAttack() {

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
			loadAnimation(attackLeft);
			break;
		case Facing::DOWN:
			loadAnimation(attackDown);
			break;
		case Facing::RIGHT:
			loadAnimation(attackRight);
			break;
		case Facing::UP:
			loadAnimation(attackUp);
			break;
	}

	repeatAttackCycles = 0;
	action = CreatureAction::ATTACKING_ACTION;
	canCancelAction = false;
}


void Chrono::initializeAssets() {
	PlayerCharacter::initializeAssets();
}

void Chrono::loadWeapon(shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {
}
