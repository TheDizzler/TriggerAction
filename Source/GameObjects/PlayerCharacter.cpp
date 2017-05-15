#include "../pch.h"
#include "PlayerCharacter.h"
#include "../Managers/MapManager.h"
#include "../GUIObjects/MenuDialog.h"
#include "../Screens/LevelScreen.h"
#include "../Engine/GameEngine.h"
#include <math.h>


const static USHORT MAX_PROJECTILES = 3;

PlayerCharacter::PlayerCharacter(shared_ptr<PlayerSlot> slot) {
	playerSlot = slot;
	joystick = playerSlot->getStick();

	characterData = slot->characterData;
	name = characterData->name;
	assetSet = characterData->assets;

	setHitbox(characterData->hitbox.get());

	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	loadAnimation("stand right");
	origin = Vector2(0, getHeight());
}

PlayerCharacter::~PlayerCharacter() {
}

void PlayerCharacter::reloadData(CharacterData* data) {

	//characterData = data;
	loadWeapon(data->weaponAssets, data->weaponPositions);
}


void PlayerCharacter::setInitialPosition(const Vector2& startingPosition) {

	setPosition(Vector3(startingPosition.x, startingPosition.y, 0));

}


void PlayerCharacter::update(double deltaTime) {

	switch (action) {
		case CreatureAction::WAITING_ACTION:
		case CreatureAction::MOVING_ACTION:
			if (joystick->bButtonStates[ControlButtons::Y]) {
				startMainAttack();
			} else {
				movement(deltaTime);

			}
			if (!stillAttacking)
				break;
			else
		case CreatureAction::ATTACKING_ACTION:
			attackUpdate(deltaTime);

			break;

	}

	if (joystick->bButtonStates[ControlButtons::START]) {

		if (!playerSlot->pauseDialog->isOpen())
			GameEngine::showCustomDialog(playerSlot->pauseDialog.get());

	}

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= currentAnimation->animationFrames.size())
			currentFrameIndex = 0;
		currentFrameTime = 0;
		Frame* frame = currentAnimation->animationFrames[currentFrameIndex].get();
		currentFrameDuration = frame->frameTime;
	}

	for (const auto& bullet : projectiles)
		bullet->update(deltaTime);
}


void PlayerCharacter::draw(SpriteBatch* batch) {

	batch->Draw(currentAnimation->texture.Get(), drawPosition,
		&currentAnimation->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		currentAnimation->animationFrames[currentFrameIndex]->origin, scale,
		SpriteEffects_None, layerDepth);

	for (const auto& bullet : projectiles)
		bullet->draw(batch);
	debugDraw(batch);
}




void PlayerCharacter::startMainAttack() {
	// get direction facing
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

	action = CreatureAction::ATTACKING_ACTION;
	currentFrameTime = 0;

	// fire bullet


	projectiles[nextBullet++]->fire(facing, position);
	if (nextBullet >= MAX_PROJECTILES)
		nextBullet = 0;
}

void PlayerCharacter::attackUpdate(double deltaTime) {

	switch (currentFrameIndex) {
		case 0: // readying attack
			break;
		case 1: // firing bullet 

			break;
		case 2: // recoil/cooldown
			break;
		case 3: // after animations (cancelable)
			action = CreatureAction::WAITING_ACTION;
			stillAttacking = true;
			break;
		case 4: // fully completed animation
			switch (facing) {
				case Facing::RIGHT:
					loadAnimation("stand right");
					break;
				case Facing::LEFT:
					loadAnimation("stand left");
					break;
				case Facing::DOWN:
					loadAnimation("stand down");
					break;
				case Facing::UP:
					loadAnimation("stand up");
					break;
			}
			stillAttacking = false;
			break;

	}
}

void PlayerCharacter::movement(double deltaTime) {
	if (getMovement(deltaTime, joystick->lAxisX, joystick->lAxisY)) {
		stillAttacking = false;
		bool collision = false;
		// check for collisions
		for (const Hitbox* hb : hitboxesAll) {
			if (hb == &hitbox)
				continue;
			if (hitbox.collision2d(hb)) {
				collision = true;
				int  hit = 69;
			}
		}

		if (collision && !lastCollision)
			debugSetTint(Color(1, .1, .1, 1));
		else if (!collision && lastCollision)
			debugSetTint(Color(0, 0, 0, 1));

		lastCollision = collision;


	} else if (!waiting) {
		waiting = true;
		moving = false;
		switch (facing) {
			case Facing::RIGHT:
				loadAnimation("stand right");
				break;
			case Facing::LEFT:
				loadAnimation("stand left");
				break;
			case Facing::DOWN:
				loadAnimation("stand down");
				break;
			case Facing::UP:
				loadAnimation("stand up");
				break;
		}
	}
}


bool PlayerCharacter::getMovement(double deltaTime, int horzDirection, int vertDirection) {

	bool runningNow = joystick->bButtonStates[ControlButtons::B];
	if (runningNow != running)
		moving = false;
	running = runningNow;
	float speedFactor;
	if (running)
		speedFactor = 1.5;
	else
		speedFactor = 1;

	if (horzDirection > 10) {
		// moving right
		if (vertDirection < -10) {
			// moving right & up
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			moveBy(Vector3(moveByX, -moveByY, 0));
		} else if (vertDirection > 10) {
			// moving right & down
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			moveBy(Vector3(moveByX, moveByY, 0));
		} else {
			float moveByX = moveRightSpeed*deltaTime*speedFactor;
			moveBy(Vector3(moveByX, 0, 0));
		}

		if (!moving || facing != Facing::RIGHT) {
			if (runningNow)
				loadAnimation("run right");
			else
				loadAnimation("walk right");
			moving = true;
			facing = Facing::RIGHT;
		}
		waiting = false;
		return true;
	}

	if (horzDirection < -10) {
		// moving left

		if (vertDirection < -10) {
			// moving left & up
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			moveBy(Vector3(-moveByX, -moveByY, 0));
		} else if (vertDirection > 10) {
			// moving left & down
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			moveBy(Vector3(-moveByX, moveByY, 0));
		} else {
			float moveByX = moveRightSpeed*deltaTime*speedFactor;
			moveBy(Vector3(-moveByX, 0, 0));

		}

		if (!moving || facing != Facing::LEFT) {
			if (runningNow)
				loadAnimation("run left");
			else
				loadAnimation("walk left");
			moving = true;
			facing = Facing::LEFT;
		}

		waiting = false;
		return true;
	}


	if (vertDirection < -10) {
		// moving up
		float moveByY = moveDownSpeed*deltaTime*speedFactor;
		moveBy(Vector3(0, -moveByY, 0));
		if (!moving || facing != Facing::UP) {
			if (runningNow)
				loadAnimation("run up");
			else
				loadAnimation("walk up");
			moving = true;
			facing = Facing::UP;
		}
		waiting = false;
		return true;
	}

	if (vertDirection > 10) {
		// moving down
		float moveByY = moveDownSpeed*deltaTime*speedFactor;
		moveBy(Vector3(0, moveByY, 0));
		if (!moving || facing != Facing::DOWN) {
			if (runningNow)
				loadAnimation("run down");
			else
				loadAnimation("walk down");
			moving = true;
			facing = Facing::DOWN;
		}
		waiting = false;
		return true;
	}

	return false;
}


void PlayerCharacter::loadWeapon(
	shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {

	projectiles.clear();

	for (int i = 0; i < MAX_PROJECTILES; ++i) {
		unique_ptr<Projectile> proj = make_unique<Projectile>(weaponPositions);
		proj->loadBullet(weaponSet->getAnimation("AirGun Bullet Left"));
		proj->loadHitEffect(weaponSet->getAnimation("AirGun HitEffect"));
		projectiles.push_back(move(proj));
	}

}