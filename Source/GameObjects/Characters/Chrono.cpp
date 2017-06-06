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

	weight = 70;

	memcpy(attackBoxSizes, characterData->attackBoxSizes, sizeof(attackBoxSizes));

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
	hitEffectManager.update(deltaTime);

#ifdef  DEBUG_HITBOXES
	attackFrame->update();
#endif //  DEBUG_HITBOXES
}

void Chrono::draw(SpriteBatch * batch) {
	PlayerCharacter::draw(batch);

	hitEffectManager.draw(batch);
#ifdef  DEBUG_HITBOXES
	if (drawAttack)
		attackFrame->draw(batch);
#endif //  DEBUG_HITBOXES
}

int awaitInputCycles = 0;
void Chrono::attackUpdate(double deltaTime) {

	currentFrameTime += deltaTime;
	switch (currentAttack) {
		case AWAIT_INPUT:
			// cycles 5 times between frame 2 and 1 until attack button pressed

#ifdef  DEBUG_HITBOXES
			drawAttack = false;
#endif //  DEBUG_HITBOXES

			waitingTime += deltaTime;
			if (joystick->yButton()) {
				canCancelAction = false;
				switch (++currentComboAttack) {
					case 2:
						secondAttackStart();
						break;
					case 3:
						thirdAttackStart();
						break;
					case 4:
						fourthAttackStart();
						break;

				}
			} else if (currentFrameTime >= currentFrameDuration) {
				if (currentFrameIndex == 2) {
					if (++awaitInputCycles > 2) {
						endAttack();
						return;
					}
					currentFrameIndex = 1;
				} else
					currentFrameIndex = 2;

				currentFrameTime = 0;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;
			}

			if (waitingTime >= CAN_CANCEL_COOLDOWN_TIME)
				canCancelAction = true;
			break;
		case FIRST_ATTACK:
			if (currentFrameTime >= currentFrameDuration) {
				if (++currentFrameIndex == 1) { // finished wind-up
					currentFrameTime = 0;
					currentFrameDuration
						= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
					currentFrameRect
						= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
					currentFrameOrigin
						= currentAnimation->animationFrames[currentFrameIndex]->origin;
					firstAttack();
				} else { // finished attack frame
#ifdef  DEBUG_HITBOXES
					drawAttack = false;
#endif //  DEBUG_HITBOXES
					currentAttack = AWAIT_INPUT;
					canCancelAction = true;
					currentFrameTime = 0;
					currentFrameDuration
						= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
					currentFrameRect
						= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
					currentFrameOrigin
						= currentAnimation->animationFrames[currentFrameIndex]->origin;
				}
			}
			break;
		case SECOND_ATTACK:
		{
			moveTime += deltaTime;
			double percentMoved = moveTime / currentFrameDuration;
			if (percentMoved < 1) {
				setPosition(Vector3::Lerp(moveStart, moveEnd, percentMoved));
				break;
			} else if (!yetAttacked) {
				yetAttacked = true;
				currentFrameIndex = 4;
				currentFrameTime = 0;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;
				secondAttack();
				break;
			}

			if (currentFrameTime >= currentFrameDuration) {
				currentAttack = AWAIT_INPUT;
				currentFrameIndex = 1;
				currentFrameTime = 0;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;
			}

		}
		break;
		case THIRD_ATTACK:
		{
			moveTime += deltaTime;
			double percentMoved = moveTime / currentFrameDuration;
			if (percentMoved <= 1) {
				setPosition(Vector3::Lerp(moveStart, moveEnd, percentMoved));
				break;
			} else if (percentMoved > 1 && !falling) {
				falling = true;
				moveTime = 0;
				moveStart = position;
				switch (facing) {
					case Facing::LEFT:
						moveEnd = position + Vector3(-15, 0, -10);
						break;
					case Facing::DOWN:
						moveEnd = position + Vector3(0, 15, -10);
						break;
					case Facing::RIGHT:
						moveEnd = position + Vector3(15, 0, -10);
						break;
					case Facing::UP:
						moveEnd = position + Vector3(0, -15, -10);
						break;
				}
				break;
			} else if (!yetAttackedThird) {
				Vector3 newPos = position;
				newPos.z = 0;
				setPosition(newPos); // insures we end on z = 0;
				yetAttackedThird = true;
				currentFrameIndex = 1;
				currentFrameTime = 0;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;
				thirdAttack();

				break;
			}

			if (currentFrameTime >= currentFrameDuration) {
				currentAttack = AWAIT_INPUT;
				currentFrameIndex = 1;
				currentFrameTime = 0;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;
			}
		}
		break;
		case FOURTH_ATTACK:
			if (!yetFourthAttack) {
				fourthAttack();
				yetFourthAttack = true;
			}
			moveTime += deltaTime;
			double percentMoved = moveTime / currentFrameDuration;
			if (percentMoved <= 1) {
				setPosition(Vector3::Lerp(position, moveEnd, percentMoved));
				currentFrameTime = 0;
				break;
			} else if (percentMoved > 1 && !falling) {
				falling = true;
				moveTime = 0;
				moveStart = position;
				currentFrameTime = 0;
				currentFrameIndex = 1;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;

				switch (facing) {
					case Facing::LEFT:
						moveEnd = position + Vector3(0, 0, -FOURTH_ATTACK_JUMP_HEIGHT);
						break;
					case Facing::DOWN:
						moveEnd = position + Vector3(0, 0, -FOURTH_ATTACK_JUMP_HEIGHT);
						break;
					case Facing::RIGHT:
						moveEnd = position + Vector3(0, 0, -FOURTH_ATTACK_JUMP_HEIGHT);
						break;
					case Facing::UP:
						moveEnd = position + Vector3(0, 0, -FOURTH_ATTACK_JUMP_HEIGHT);
						break;
				}
				break;
			}
			Vector3 newPos = position;
			newPos.z = 0;
			setPosition(newPos); // insures we end on z = 0;
			if (currentFrameTime >= currentFrameDuration) {
				if (++currentFrameIndex > 2)
					currentFrameIndex = 1;
				currentFrameTime = 0;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;
			}
			fourthAttackCooldownTime += deltaTime;
			if (fourthAttackCooldownTime >= FOURTH_ATTACK_COOLDOWN_MAX) {
				endAttack();
			}
			break;
	}
}

void Chrono::endAttack() {
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
	currentAttack = NONE;

#ifdef  DEBUG_HITBOXES
	drawAttack = false;
#endif //  DEBUG_HITBOXES
}

void Chrono::firstAttack() {

	awaitInputCycles = 0;
	attackBox.size = attackBoxSizes[facing];
	attackBox.position = position;
	switch (facing) {
		case Facing::LEFT:
			attackBox.position.x -= (currentFrameOrigin.x);
			attackBox.position.y += 5;
			break;
		case Facing::DOWN:
			attackBox.position.y += attackBoxOffset.y;
			attackBox.position.x -= (getWidth() + currentFrameOrigin.x) / 2 - 2;
			break;
		case Facing::RIGHT:
			attackBox.position.x += (currentFrameOrigin.x);
			attackBox.position.y += 5;
			break;
		case Facing::UP:
			attackBox.position.x -= (getWidth() + currentFrameOrigin.x) / 2 - 2;
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

		if (attackBox.collision(object->getHitbox())) {
			object->takeDamage(5);
			switch (facing) {
				case Facing::DOWN:
					object->knockBack(Vector3(0, 10, 0), weight);
					break;
				case Facing::LEFT:
					object->knockBack(Vector3(-10, 0, 0), weight);
					break;
				case Facing::UP:
					object->knockBack(Vector3(0, -10, 0), weight);
					break;
				case Facing::RIGHT:
					object->knockBack(Vector3(10, 0, 0), weight);
					break;
			}
			// slash effect
			hitEffectManager.newEffect(facing, position, 0);
		}
	}

}

void Chrono::secondAttackStart() {

	currentFrameIndex = 3;
	currentAttack = SECOND_ATTACK;
	yetAttacked = false;
	moveTime = 0;
	waitingTime = 0;

	moveStart = position;
	switch (facing) {
		case Facing::LEFT:
			moveEnd = position + Vector3(-15, 0, 0);
			break;
		case Facing::DOWN:
			moveEnd = position + Vector3(0, 15, 0);
			break;
		case Facing::RIGHT:
			moveEnd = position + Vector3(15, 0, 0);
			break;
		case Facing::UP:
			moveEnd = position + Vector3(0, -15, 0);
			break;
	}

	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect = currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin = currentAnimation->animationFrames[currentFrameIndex]->origin;

}

void Chrono::secondAttack() {

	awaitInputCycles = 0;
	attackBox.size = attackBoxSizes[facing];
	attackBox.position = position;
	switch (facing) {
		case Facing::LEFT:
			attackBox.position.x -= (currentFrameOrigin.x + attackBox.size.x);
			attackBox.position.y += 5;
			break;
		case Facing::DOWN:
			attackBox.position.y += attackBoxOffset.y;
			attackBox.position.x -= (/*getWidth() + */currentFrameOrigin.x) / 2;
			break;
		case Facing::RIGHT:
			attackBox.position.x += (currentFrameOrigin.x);
			attackBox.position.y += 5;
			break;
		case Facing::UP:
			attackBox.position.x -= (/*getWidth() + */currentFrameOrigin.x) / 2 + 6;
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

		if (attackBox.collision(object->getHitbox())) {
			object->takeDamage(5);
			switch (facing) {
				case Facing::DOWN:
					object->knockBack(Vector3(0, 10, 2), weight);
					break;
				case Facing::LEFT:
					object->knockBack(Vector3(-10, 0, 2), weight);
					break;
				case Facing::UP:
					object->knockBack(Vector3(0, -10, 2), weight);
					break;
				case Facing::RIGHT:
					object->knockBack(Vector3(10, 0, 2), weight);
					break;
			}
			// slash effect
			hitEffectManager.newEffect(facing, position, 1);
		}
	}

}

void Chrono::thirdAttackStart() {

	currentFrameIndex = 0;
	currentAttack = THIRD_ATTACK;
	yetAttackedThird = false;
	moveTime = 0;
	waitingTime = 0;


	moveStart = position;
	switch (facing) {
		case Facing::LEFT:
			moveEnd = position + Vector3(-15, 0, 10);
			//currentAnimation = attackLeft;
			break;
		case Facing::DOWN:
			moveEnd = position + Vector3(0, 15, 10);
			//currentAnimation = attackDown;
			break;
		case Facing::RIGHT:
			moveEnd = position + Vector3(15, 0, 10);
			//currentAnimation = attackRight;
			break;
		case Facing::UP:
			moveEnd = position + Vector3(0, -15, 10);
			//currentAnimation = attackUp;
			break;
	}

	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect = currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin = currentAnimation->animationFrames[currentFrameIndex]->origin;

}

void Chrono::thirdAttack() {

	awaitInputCycles = 0;
	attackBox.size = attackBoxSizes[facing];
	attackBox.position = position;
	switch (facing) {
		case Facing::LEFT:
			attackBox.position.x -= (currentFrameOrigin.x/* + attackBox.size.x*/);
			attackBox.position.y += 5;
			break;
		case Facing::DOWN:
			attackBox.size.x /= 2;
			attackBox.position.y += attackBoxOffset.y;
			//attackBox.position.x -= (getWidth() /*+ currentFrameOrigin.x*/) / 2;
			attackBox.position.x -= attackBox.size.x / 2;
			break;
		case Facing::RIGHT:
			attackBox.position.x += (currentFrameOrigin.x);
			attackBox.position.y += 5;
			break;
		case Facing::UP:
			attackBox.size.x /= 2;
			//attackBox.position.x -= (getWidth() + currentFrameOrigin.x) / 2;
			attackBox.position.x -= attackBox.size.x / 2;
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

		if (attackBox.collision(object->getHitbox())) {
			object->takeDamage(5);
			switch (facing) {
				case Facing::DOWN:
					object->knockBack(Vector3(0, 1, 20), weight);
					break;
				case Facing::LEFT:
					object->knockBack(Vector3(-1, 0, 20), weight);
					break;
				case Facing::UP:
					object->knockBack(Vector3(0, -1, 20), weight);
					break;
				case Facing::RIGHT:
					object->knockBack(Vector3(1, 0, 20), weight);
					break;
			}
			// slash effect
			hitEffectManager.newEffect(facing, position, 0);
		}
	}


}

void Chrono::fourthAttackStart() {

	currentFrameIndex = 4;
	currentAttack = FOURTH_ATTACK;

	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect = currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin = currentAnimation->animationFrames[currentFrameIndex]->origin;

	fourthAttackCooldownTime = 0;
	waitingTime = 0;
	moveTime = 0;
	moveStart = position;
	moveEnd = position + Vector3(0, 0, FOURTH_ATTACK_JUMP_HEIGHT);
	yetFourthAttack = false;
	falling = false;
}

void Chrono::fourthAttack() {

	awaitInputCycles = 0;
	attackBox.size = attackBoxSizes[facing];
	attackBox.position = position;
	switch (facing) {
		case Facing::LEFT:
			attackBox.position.x -= (currentFrameOrigin.x + attackBox.size.x);
			attackBox.position.y += 5;
			break;
		case Facing::DOWN:
			attackBox.position.y += attackBoxOffset.y;
			attackBox.position.x -= (/*getWidth() + */currentFrameOrigin.x) / 2 - 2;
			break;
		case Facing::RIGHT:
			attackBox.position.x += (currentFrameOrigin.x);
			attackBox.position.y += 5;
			break;
		case Facing::UP:
			attackBox.position.x -= (getWidth() /*+ currentFrameOrigin.x*/) / 2 - 4;
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

		if (attackBox.collision(object->getHitbox())) {
			object->takeDamage(5);
			// slash effect
			hitEffectManager.newEffect(facing, position, 2);
		}
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

	currentAttack = FIRST_ATTACK;
	awaitInputCycles = 0;
	action = CreatureAction::ATTACKING_ACTION;
	canCancelAction = false;
	currentComboAttack = 1;
	yetAttacked = false;
	yetAttackedThird = false;
	moving = false;
}


void Chrono::initializeAssets() {
	PlayerCharacter::initializeAssets();
}

void Chrono::loadWeapon(shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {
	hitEffectManager.loadHitEffects(weaponSet);
}




HitEffect::HitEffect(const Vector3& pos) {
	position = pos;
}

HitEffect::~HitEffect() {
}

void HitEffect::load(GraphicsAsset* const graphicsAsset) {
	texture = graphicsAsset->getTexture();
	width = graphicsAsset->getWidth();
	height = graphicsAsset->getHeight();

	origin = graphicsAsset->getOrigin();

	sourceRect = graphicsAsset->getSourceRect();
}

const float MAX_LIVE_TIME = 1.5;
bool HitEffect::update(double deltaTime) {
	timeLive += deltaTime;
	if (timeLive >= MAX_LIVE_TIME) {
		// die die die

		return false;
	}
	return true;
}

void HitEffect::draw(SpriteBatch* batch) {


	batch->Draw(texture.Get(), position,
		&sourceRect, tint, rotation,
		origin, scale, spriteEffect, layerDepth);
}

const int HitEffect::getWidth() const {
	return sourceRect.right - sourceRect.left;
}

const int HitEffect::getHeight() const {
	return sourceRect.bottom - sourceRect.top;
}



void HitEffectManager::loadHitEffects(shared_ptr<AssetSet> weaponSet) {

	hitEffects[Facing::DOWN].clear();
	hitEffects[Facing::DOWN].push_back(weaponSet->getAsset("Wooden Sword Down 1"));
	hitEffects[Facing::DOWN].push_back(weaponSet->getAsset("Wooden Sword Down 2"));
	hitEffects[Facing::DOWN].push_back(weaponSet->getAsset("Wooden Sword Down 3"));

	hitEffects[Facing::LEFT].clear();
	hitEffects[Facing::LEFT].push_back(weaponSet->getAsset("Wooden Sword Left 1"));
	hitEffects[Facing::LEFT].push_back(weaponSet->getAsset("Wooden Sword Left 2"));
	hitEffects[Facing::LEFT].push_back(weaponSet->getAsset("Wooden Sword Left 3"));

	hitEffects[Facing::UP].clear();
	hitEffects[Facing::UP].push_back(weaponSet->getAsset("Wooden Sword Up 1"));
	hitEffects[Facing::UP].push_back(weaponSet->getAsset("Wooden Sword Up 2"));
	hitEffects[Facing::UP].push_back(weaponSet->getAsset("Wooden Sword Up 3"));

	hitEffects[Facing::RIGHT].clear();
	hitEffects[Facing::RIGHT].push_back(weaponSet->getAsset("Wooden Sword Right 1"));
	hitEffects[Facing::RIGHT].push_back(weaponSet->getAsset("Wooden Sword Right 2"));
	hitEffects[Facing::RIGHT].push_back(weaponSet->getAsset("Wooden Sword Right 3"));
}

void HitEffectManager::update(double deltaTime) {
	for (int i = 0; i < liveEffects.size(); ) {
		if (!liveEffects[i].update(deltaTime)) {
			swap(liveEffects[i], liveEffects.back());
			liveEffects.pop_back();
			continue;
		}
		++i;
	}
}

void HitEffectManager::draw(SpriteBatch* batch) {
	for (HitEffect hitEffect : liveEffects) {
		hitEffect.draw(batch);
	}
}

void HitEffectManager::newEffect(Facing facing, const Vector3& position, USHORT hitNumber) {

	HitEffect fx(position);
	fx.load(hitEffects[facing][hitNumber]);

	liveEffects.push_back(fx);
}
