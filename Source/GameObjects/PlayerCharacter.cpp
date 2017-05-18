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

	walkDown = assetSet->getAnimation("walk down");
	walkLeft = assetSet->getAnimation("walk left");
	walkUp = assetSet->getAnimation("walk up");
	walkRight = assetSet->getAnimation("walk right");

	attackDown = assetSet->getAnimation("attack down");
	attackLeft = assetSet->getAnimation("attack left");
	attackUp = assetSet->getAnimation("attack up");
	attackRight = assetSet->getAnimation("attack right");


	provoke = assetSet->getAnimation("provoke");
	surprise = assetSet->getAnimation("surprise");
	hit = assetSet->getAnimation("hit");

	setHitbox(characterData->hitbox.get());
	radarBox = Hitbox(hitbox);

	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	loadAnimation("stand right");
	currentFrameTexture = currentAnimation->texture.Get();
}

PlayerCharacter::~PlayerCharacter() {
}

void PlayerCharacter::reloadData(CharacterData* data) {

	characterData = data;
	setHitbox(characterData->hitbox.get());
	setHitboxPosition(position);
	radarBox = Hitbox(hitbox);
	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);
}


void PlayerCharacter::setInitialPosition(const Vector2& startingPosition) {

	setPosition(Vector3(startingPosition.x, startingPosition.y, 0));

}


void PlayerCharacter::update(double deltaTime) {

	switch (action) {
		case CreatureAction::ATTACKING_ACTION:
			attackUpdate(deltaTime);
			if (canCancelAction) {
				if (joystick->bButtonStates[ControlButtons::Y]) {
					startMainAttack();
				} else {
					movement(deltaTime);
					moveUpdate(deltaTime);
				}
			}
			break;
		case CreatureAction::WAITING_ACTION:
		default:
			waitUpdate(deltaTime);
		case CreatureAction::MOVING_ACTION:
			if (joystick->bButtonStates[ControlButtons::Y]) {
				startMainAttack();
			} else {
				movement(deltaTime);
				moveUpdate(deltaTime);
			}

			break;


		case CreatureAction::HIT_ACTION:
			hitUpdate(deltaTime);
			break;


	}

	if (joystick->bButtonStates[ControlButtons::START]) {

		if (!playerSlot->pauseDialog->isOpen())
			GameEngine::showCustomDialog(playerSlot->pauseDialog.get());

	}

	debugUpdate();

	for (const auto& bullet : projectiles)
		bullet->update(deltaTime);
}


void PlayerCharacter::draw(SpriteBatch* batch) {

	batch->Draw(currentFrameTexture, drawPosition,
		&currentFrameRect, tint, rotation,
		currentFrameOrigin, scale,
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
	currentFrameIndex = -1;
	currentFrameTime = currentFrameDuration;
	action = CreatureAction::ATTACKING_ACTION;
	moving = false;
	canCancelAction = false;
	attackUpdate(0);
}

const int ANIMATION_REPEATS = 1;
void PlayerCharacter::attackUpdate(double deltaTime) {

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
					currentFrameDuration = currentFrameTime =
						currentAnimation->animationFrames[currentFrameIndex]->frameTime;
					currentFrameRect
						= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
					currentFrameOrigin
						= currentAnimation->animationFrames[currentFrameIndex]->origin;
					return;
				}

				canCancelAction = true;
				break;
			case 4:
				break;
			case 5:
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


void PlayerCharacter::waitUpdate(double deltaTime) {

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= currentAnimation->animationFrames.size())
			currentFrameIndex = 0;
		currentFrameTime = 0;
		currentFrameDuration
			= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
		currentFrameRect
			= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
		currentFrameOrigin
			= currentAnimation->animationFrames[currentFrameIndex]->origin;
	}
}


void PlayerCharacter::movement(double deltaTime) {

	Vector3 moveVector = getMovement(deltaTime, joystick->lAxisX, joystick->lAxisY);
		//if (getMovement(deltaTime, joystick->lAxisX, joystick->lAxisY)) {
	if (moveVector != Vector3::Zero) {

		radarBox.position = hitbox.position + moveVector * 2;

		bool collision = false;
		// check for collisions
		for (const Tangible* hb : hitboxesAll) {
			if (hb->getHitbox() == &hitbox)
				continue;
			if (radarBox.collision2d(hb->getHitbox())) {
				collision = true;
				break;
			}
		}

		radarBox.position = hitbox.position;
		if (collision && moveVector.x != 0 && moveVector.y != 0) {
			collision = false;
			//test if can move other directions
			Vector3 testVector = moveVector;
			testVector.x = 0;
			radarBox.position = hitbox.position + testVector * 2;
			for (const Tangible* hb : hitboxesAll) {
				if (hb->getHitbox() == &hitbox)
					continue;
				if (radarBox.collision2d(hb->getHitbox())) {
					collision = true;
					break;
				}
			}

			if (!collision)
				moveBy(testVector);
			else {
				collision = false;
				testVector = moveVector;
				testVector.y = 0;
				radarBox.position = hitbox.position + testVector * 2;
				for (const Tangible* hb : hitboxesAll) {
					if (hb->getHitbox() == &hitbox)
						continue;
					if (radarBox.collision2d(hb->getHitbox())) {
						collision = true;
						break;
					}
				}
				if (!collision)
					moveBy(testVector);
			}

		/*if (collision && !lastCollision)
			debugSetTint(Color(1, .1, .1, 1));
		*/
		} else if (!collision) {
			moveBy(moveVector);
			//if (lastCollision)
				//debugSetTint(Color(0, 0, 0, 1));
		}
		//lastCollision = collision;
		action = CreatureAction::MOVING_ACTION;

	} else if (!waiting) { // redo this to match current command flow
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


Vector3 PlayerCharacter::getMovement(double deltaTime, int horzDirection, int vertDirection) {

	bool runningNow = joystick->bButtonStates[ControlButtons::B];
	if (runningNow != running)
		moving = false;
	running = runningNow;
	float speedFactor;
	if (running)
		speedFactor = 1.5;
	else
		speedFactor = 1;

	Vector3 moveVector = Vector3::Zero;
	if (horzDirection > 10) {
		// moving right
		if (vertDirection < -10) {
			// moving right & up
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			//moveBy(Vector3(moveByX, -moveByY, 0));
			moveVector = Vector3(moveByX, -moveByY, 0);
		} else if (vertDirection > 10) {
			// moving right & down
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			//moveBy(Vector3(moveByX, moveByY, 0));
			moveVector = Vector3(moveByX, moveByY, 0);
		} else {
			float moveByX = moveRightSpeed*deltaTime*speedFactor;
			//moveBy(Vector3(moveByX, 0, 0));
			moveVector = Vector3(moveByX, 0, 0);
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
		return moveVector;
	}

	if (horzDirection < -10) {
		// moving left

		if (vertDirection < -10) {
			// moving left & up
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			//moveBy(Vector3(-moveByX, -moveByY, 0));
			moveVector = Vector3(-moveByX, -moveByY, 0);
		} else if (vertDirection > 10) {
			// moving left & down
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			//moveBy(Vector3(-moveByX, moveByY, 0));
			moveVector = Vector3(-moveByX, moveByY, 0);
		} else {
			float moveByX = moveRightSpeed*deltaTime*speedFactor;
			//moveBy(Vector3(-moveByX, 0, 0));
			moveVector = Vector3(-moveByX, 0, 0);
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
		return moveVector;
	}


	if (vertDirection < -10) {
		// moving up
		float moveByY = moveDownSpeed*deltaTime*speedFactor;
		//moveBy(Vector3(0, -moveByY, 0));
		moveVector = Vector3(0, -moveByY, 0);
		if (!moving || facing != Facing::UP) {
			if (runningNow)
				loadAnimation("run up");
			else
				loadAnimation("walk up");
			moving = true;
			facing = Facing::UP;
		}
		waiting = false;
		return moveVector;
	}

	if (vertDirection > 10) {
		// moving down
		float moveByY = moveDownSpeed*deltaTime*speedFactor;
		//moveBy(Vector3(0, moveByY, 0));
		moveVector = Vector3(0, moveByY, 0);
		if (!moving || facing != Facing::DOWN) {
			if (runningNow)
				loadAnimation("run down");
			else
				loadAnimation("walk down");
			moving = true;
			facing = Facing::DOWN;
		}
		waiting = false;
		return moveVector;
	}

	return moveVector;
}


void PlayerCharacter::loadWeapon(
	shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {

	projectiles.clear();

	for (int i = 0; i < MAX_PROJECTILES; ++i) {
		unique_ptr<Projectile> proj = make_unique<Projectile>(this, weaponPositions);
		proj->loadBullet(weaponSet->getAnimation("AirGun Bullet Left"),
			weaponSet->getAsset("Bullet Shadow"));
		proj->loadHitEffect(weaponSet->getAnimation("AirGun HitEffect"));
		projectiles.push_back(move(proj));
	}

}