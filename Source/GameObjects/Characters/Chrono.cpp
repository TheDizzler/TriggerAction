#include "../../pch.h"
#include "Chrono.h"

#include "../../Engine/GameEngine.h"
Chrono::Chrono(shared_ptr<PlayerSlot> slot) : PlayerCharacter(slot) {
	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	initializeAssets();

	loadAnimation(standRight);
	currentFrameTexture = currentAnimation->texture.Get();

	currentHP = maxHP = 75;
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
	if (drawAttackBox)
		attackFrame->draw(batch);
#endif //  DEBUG_HITBOXES
}

int awaitInputCycles = 0;
void Chrono::attackUpdate(double deltaTime) {

	currentFrameTime += deltaTime;
	switch (currentAttack) {
		case AWAIT_INPUT:
		{
			// cycles 5 times between frame 2 and 1 until attack button pressed

#ifdef  DEBUG_HITBOXES
			drawAttackBox = false;
#endif //  DEBUG_HITBOXES

			waitingTime += deltaTime;
			if (joystick->yButtonPushed()) {
				canCancelAction = false;
				switch (++currentComboAttack) {
					case 2:
						secondAttackStart();
						break;
					case 3:
						thirdAttackStart();
						position -= GRAVITY * (deltaTime + .0000001);
						break;
					case 4:
						fourthAttackStart();
						position -= GRAVITY * (deltaTime + .0000001);
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
		}
		break;
		case FIRST_ATTACK:
		{
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
					drawAttackBox = false;
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
		}
		break;
		case SECOND_ATTACK:
		{

			if (!yetAttacked) {
				Vector3 moveVector = moveVelocity * deltaTime;
				radarBox.position = hitbox.position + moveVector * 2;

				//bool collision = false;
				// check for collisions
				for (Tangible* hb : tangiblesAll) {
					if (hb == this)
						continue;
					if (radarBox.collision(hb->getHitbox())) {
						hb->knockBack(moveVector);
						hb->takeDamage(0, false);
						moveVelocity.x = 0;
						moveVelocity.y = 0;
						falling = true;
						break;
					}
				}


				if (currentFrameTime >= currentFrameDuration) {
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
				}
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
			Vector3 moveVector = moveVelocity * deltaTime;
			if (!falling) {

				if (!yetAttackedThird) {

					currentFrameIndex = 1;
					currentFrameTime = 0;
					currentFrameDuration
						= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
					currentFrameRect
						= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
					currentFrameOrigin
						= currentAnimation->animationFrames[currentFrameIndex]->origin;

					thirdAttack();
					yetAttackedThird = true;
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

				break;
			} else if (falling) {
				radarBox.position = hitbox.position + moveVector * 2;
				bool collision = false;
				// check for collisions
				for (Tangible* hb : tangiblesAll) {
					if (hb == this)
						continue;
					if (radarBox.collision(hb->getHitbox())) {
						collision = true; // it's POSSIBLE that more than one object could collide
						/*moveVector.x = 0;
						moveVector.y = 0;*/
						moveVelocity.x = 0;
						moveVelocity.y = 0;
						hb->knockBack(moveVector);
						hb->takeDamage(0, false);

						break;
					}
				}
				//moveBy(moveVector);
				break;
			}


		}
		break;
		case FOURTH_ATTACK:
		{

			fourthAttack(deltaTime);

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

	hitList.clear();

#ifdef  DEBUG_HITBOXES
	drawAttackBox = false;
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
			attackBox.position.y -= attackBoxOffset.y / 2;
			break;
	}


	attackBox.position.y -= attackBox.size.y;

#ifdef  DEBUG_HITBOXES
	drawAttackBox = true;
	attackFrame->setSize(Vector2(attackBox.size.x, attackBox.size.y));
	attackFrame->setPosition(Vector2(
		attackBox.position.x, attackBox.position.y));
#endif //  DEBUG_HITBOXES

	// hit detection
	for (Tangible* object : tangiblesAll) {
		if (object == this) {
			continue;
		}

		if (attackBox.collision(object->getHitbox())) {
			switch (facing) {
				case Facing::DOWN:
					object->knockBack(Vector3(0, 100, 0), weight);
					break;
				case Facing::LEFT:
					object->knockBack(Vector3(-100, 0, 0), weight);
					break;
				case Facing::UP:
					object->knockBack(Vector3(0, -100, 0), weight);
					break;
				case Facing::RIGHT:
					object->knockBack(Vector3(100, 0, 0), weight);
					break;
			}
			object->takeDamage(2);

			// slash effect
			hitEffectManager.newEffect(facing, position, 0);
		}
	}

}

void Chrono::secondAttackStart() {

	currentFrameIndex = 3;
	currentFrameTime = 0;
	currentAttack = SECOND_ATTACK;
	yetAttacked = false;
	moveTime = 0;
	waitingTime = 0;

	switch (facing) {
		case Facing::LEFT:
			moveVelocity = Vector3(-75, 0, 0);
			break;
		case Facing::DOWN:
			moveVelocity = Vector3(0, 75, 0);
			break;
		case Facing::RIGHT:
			moveVelocity = Vector3(75, 0, 0);
			break;
		case Facing::UP:
			moveVelocity = Vector3(0, -75, 0);
			break;
	}

	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect = currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin = currentAnimation->animationFrames[currentFrameIndex]->origin;

}

void Chrono::secondAttack() {

	moveVelocity.x = 0;
	moveVelocity.y = 0;

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
			attackBox.position.x -= (currentFrameOrigin.x) / 2;
			break;
		case Facing::RIGHT:
			attackBox.position.x += (currentFrameOrigin.x);
			attackBox.position.y += 5;
			break;
		case Facing::UP:
			attackBox.position.x -= (currentFrameOrigin.x) / 2 + 6;
			attackBox.position.y -= attackBoxOffset.y / 2;
			break;
	}


	attackBox.position.y -= attackBox.size.y;

#ifdef  DEBUG_HITBOXES
	drawAttackBox = true;
	attackFrame->setSize(Vector2(attackBox.size.x, attackBox.size.y));
	attackFrame->setPosition(Vector2(
		attackBox.position.x, attackBox.position.y));
#endif //  DEBUG_HITBOXES

	// hit detection
	for (Tangible* object : tangiblesAll) {
		if (object == this) {
			continue;
		}

		if (attackBox.collision(object->getHitbox())) {
			switch (facing) {
				case Facing::DOWN:
					object->knockBack(Vector3(0, 200, 25), weight);
					break;
				case Facing::LEFT:
					object->knockBack(Vector3(-200, 0, 25), weight);
					break;
				case Facing::UP:
					object->knockBack(Vector3(0, -200, 25), weight);
					break;
				case Facing::RIGHT:
					object->knockBack(Vector3(200, 0, 25), weight);
					break;
			}
			object->takeDamage(4);

			// slash effect
			hitEffectManager.newEffect(facing, position, 1);
		}
	}

}

void Chrono::thirdAttackStart() {

	currentFrameIndex = 0;
	currentAttack = THIRD_ATTACK;
	//yetAttackedThird = false;
	moveTime = 0;
	waitingTime = 0;
	falling = true;

	Vector3 direction = Vector3::Zero;

	//moveStart = position;
	switch (facing) {
		case Facing::LEFT:
			direction = Vector3(-150, 0, 200);
			//moveEnd = position + Vector3(-15, 0, 10);
			break;
		case Facing::DOWN:
			direction = Vector3(0, 150, 200);
			//moveEnd = position + Vector3(0, 15, 10);
			break;
		case Facing::RIGHT:
			direction = Vector3(150, 0, 200);
			//moveEnd = position + Vector3(15, 0, 10);
			break;
		case Facing::UP:
			direction = Vector3(0, -150, 200);
			//moveEnd = position + Vector3(0, -15, 10);
			break;
	}

	moveVelocity = direction;

	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect = currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin = currentAnimation->animationFrames[currentFrameIndex]->origin;

}

void Chrono::thirdAttack() {

	moveVelocity.x = 0;
	moveVelocity.y = 0;

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
			attackBox.position.x -= attackBox.size.x / 2;
			break;
		case Facing::RIGHT:
			attackBox.position.x += (currentFrameOrigin.x);
			attackBox.position.y += 5;
			break;
		case Facing::UP:
			attackBox.size.x /= 2;
			attackBox.position.x -= attackBox.size.x / 2;
			attackBox.position.y -= attackBoxOffset.y / 2;
			break;
	}


	attackBox.position.y -= attackBox.size.y;

#ifdef  DEBUG_HITBOXES
	drawAttackBox = true;
	attackFrame->setSize(Vector2(attackBox.size.x, attackBox.size.y));
	attackFrame->setPosition(Vector2(
		attackBox.position.x, attackBox.position.y));
#endif //  DEBUG_HITBOXES

	// hit detection
	for (Tangible* object : tangiblesAll) {
		if (object == this) {
			continue;
		}

		if (attackBox.collision(object->getHitbox())) {
			switch (facing) {
				case Facing::DOWN:
					object->knockBack(Vector3(0, 100, -100), weight);
					break;
				case Facing::LEFT:
					object->knockBack(Vector3(-100, 0, -100), weight);
					break;
				case Facing::UP:
					object->knockBack(Vector3(0, -100, -100), weight);
					break;
				case Facing::RIGHT:
					object->knockBack(Vector3(100, 0, -100), weight);
					break;
			}
			object->takeDamage(5);

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

	moveVelocity = Vector3(0, 0, 300);

	awaitInputCycles = 0;
	attackBox.size = attackBoxSizes[facing];
	attackBox.position = position;
	switch (facing) {
		case Facing::LEFT:
			attackBox.position.x -= (currentFrameOrigin.x + attackBox.size.x);
			attackBox.position.y += 5;
			break;
		case Facing::DOWN:
			attackBox.position.y += attackBoxOffset.y + 4;
			attackBox.position.x -= (currentFrameOrigin.x) / 2 - 2;
			break;
		case Facing::RIGHT:
			attackBox.position.x += (currentFrameOrigin.x);
			attackBox.position.y += 5;
			break;
		case Facing::UP:
			attackBox.position.x -= (getWidth()) / 2 - 4;
			attackBox.position.y -= attackBox.size.y / 2;
			break;
	}


	attackBox.position.y -= attackBox.size.y;
	falling = true;

#ifdef  DEBUG_HITBOXES
	drawAttackBox = true;
	attackFrame->setSize(Vector2(attackBox.size.x, attackBox.size.y));
	attackFrame->setPosition(Vector2(
		attackBox.position.x, attackBox.position.y));
#endif //  DEBUG_HITBOXES
}

void Chrono::fourthAttack(double deltaTime) {

	if (!falling) {
		fourthAttackCooldownTime += deltaTime;
		if (fourthAttackCooldownTime >= FOURTH_ATTACK_COOLDOWN_MAX) {
			endAttack();
		}
		return;
	}

	Vector3 moveVector = moveVelocity * deltaTime;
	if (abs(fallVelocity.z) < moveVector.z) { // raising

		//moveBy(moveVector);
		attackBox.position += moveVector;

		// hit detection
		for (Tangible* object : tangiblesAll) {
			if (object == this) {
				continue;
			}

			if (std::find(hitList.begin(), hitList.end(), object) != hitList.end())
				continue;

			if (attackBox.collision(object->getHitbox())) {
				switch (facing) {
					case Facing::DOWN:
						object->knockBack(Vector3(0, 50, 250), weight);
						break;
					case Facing::LEFT:
						object->knockBack(Vector3(-50, 0, 250), weight);
						break;
					case Facing::UP:
						object->knockBack(Vector3(0, -50, 250), weight);
						break;
					case Facing::RIGHT:
						object->knockBack(Vector3(50, 0, 250), weight);
						break;
				}
				object->takeDamage(5);
				hitList.push_back(object);
					// slash effect
				hitEffectManager.newEffect(facing, position, 2);
			}
		}

	} else {  // falling
#ifdef  DEBUG_HITBOXES
		drawAttackBox = false;
#endif //  DEBUG_HITBOXES
	}
}


void Chrono::startMainAttack() {

	moveVelocity.x = 0;
	moveVelocity.y = 0;

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
	yetFourthAttack = false;
	moving = false;
}


void Chrono::initializeAssets() {
	PlayerCharacter::initializeAssets();
}

void Chrono::loadWeapon(shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {
	hitEffectManager.loadHitEffects(weaponSet);
}



const double MAX_LIVE_TIME = 1.1;
const double FADE_WAIT_TIME = .5;
HitEffect::HitEffect(const Vector3& pos) {
	position = pos;
	timeLive = -FADE_WAIT_TIME;
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


bool HitEffect::update(double deltaTime) {
	timeLive += deltaTime;
	tint = Color::Lerp(Color(1, 1, 1, 1), Color(.5, .1, .1, 0), timeLive / MAX_LIVE_TIME);
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
