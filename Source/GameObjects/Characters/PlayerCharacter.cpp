#include "../../pch.h"
#include "PlayerCharacter.h"
#include "../../Managers/MapManager.h"
#include "../../GUIObjects/MenuDialog.h"
#include "../../Screens/LevelScreen.h"
#include "../../Engine/GameEngine.h"
#include <math.h>




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

	jumpDown = assetSet->getAnimation("jump down");
	jumpLeft = assetSet->getAnimation("jump left");
	jumpUp = assetSet->getAnimation("jump up");
	jumpRight = assetSet->getAnimation("jump right");

	provoke = assetSet->getAnimation("provoke");
	surprise = assetSet->getAnimation("surprise");
	hit = assetSet->getAnimation("hit");

	shadow.load(assetSet->getAsset("shadow"));

	setHitbox(characterData->hitbox.get());
	radarBox = Hitbox(hitbox);

	

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
				}
			}
			break;
		case CreatureAction::WAITING_ACTION:
		default:
			waitUpdate(deltaTime);
		case CreatureAction::MOVING_ACTION:
			if (joystick->bButtonStates[ControlButtons::Y]) {
				startMainAttack();
			} else if (joystick->bButtonStates[ControlButtons::X]) {
				startJump();
			} else {
				movement(deltaTime);
				moveUpdate(deltaTime);
			}

			break;

		case CreatureAction::JUMP_ACTION:

			jumpUpdate(deltaTime);
			break;
		case CreatureAction::HIT_ACTION:
			hitUpdate(deltaTime);
			break;


	}

	if (joystick->bButtonStates[ControlButtons::START]) {

		if (!playerSlot->pauseDialog->isOpen())
			GameEngine::showCustomDialog(playerSlot->pauseDialog.get());

	}

#ifdef  DEBUG_HITBOXES
	debugUpdate();
#endif //  DEBUG_HITBOXES

}


void PlayerCharacter::draw(SpriteBatch* batch) {

	batch->Draw(currentFrameTexture, drawPosition,
		&currentFrameRect, tint, rotation,
		currentFrameOrigin, scale,
		SpriteEffects_None, layerDepth);

	shadow.draw(batch);

#ifdef  DEBUG_HITBOXES
	debugDraw(batch);
#endif //  DEBUG_HITBOXES
}


const double NORMAL_SPEED = 1.0;
const double RUN_SPEED = 1.5;
const double JUMP_TIME = .375;
const int MAX_JUMP_HEIGHT = 20;
//const int MAX_JUMP_DISTANCE = 30;
void PlayerCharacter::startJump() {

	jumpByX = jumpByY = 0;

	float speedFactor;
	if (running)
		speedFactor = RUN_SPEED;
	else
		speedFactor = NORMAL_SPEED;

	int horzDirection = joystick->lAxisX;
	int vertDirection = joystick->lAxisY;
	if (horzDirection > 10) {
		// moving right
		facing = Facing::RIGHT;
		loadAnimation(jumpRight);
		if (vertDirection < -10) {
			// moving right & up
			jumpByX = moveDiagonalRight*JUMP_TIME*speedFactor;
			jumpByY = -moveDiagonalDown*JUMP_TIME*speedFactor;
			endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, MAX_JUMP_HEIGHT);
		} else if (vertDirection > 10) {
			// moving right & down
			jumpByX = moveDiagonalRight*JUMP_TIME*speedFactor;
			jumpByY = moveDiagonalDown*JUMP_TIME*speedFactor;
			endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, MAX_JUMP_HEIGHT);
		} else {
			jumpByX = moveRightSpeed*JUMP_TIME*speedFactor;
			endHalfJumpPosition = position + Vector3(jumpByX, 0, MAX_JUMP_HEIGHT);
		}
	} else if (horzDirection < -10) {
		// moving left
		loadAnimation(jumpLeft);
		facing = Facing::LEFT;
		if (vertDirection < -10) {
			// moving left & up
			jumpByX = -moveDiagonalRight*JUMP_TIME*speedFactor;
			jumpByY = -moveDiagonalDown*JUMP_TIME*speedFactor;
			endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, MAX_JUMP_HEIGHT);
		} else if (vertDirection > 10) {
			// moving left & down
			jumpByX = -moveDiagonalRight*JUMP_TIME*speedFactor;
			jumpByY = moveDiagonalDown*JUMP_TIME*speedFactor;
			endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, MAX_JUMP_HEIGHT);
		} else {
			jumpByX = -moveRightSpeed*JUMP_TIME*speedFactor;
			endHalfJumpPosition = position + Vector3(jumpByX, 0, MAX_JUMP_HEIGHT);
		}
	} else if (vertDirection < -10) {
		// moving up
		facing = Facing::UP;
		loadAnimation(jumpUp);
		jumpByY = -moveDownSpeed*JUMP_TIME*speedFactor;
		endHalfJumpPosition = position + Vector3(0, jumpByY, MAX_JUMP_HEIGHT);
	} else if (vertDirection > 10) {
		// moving down
		facing = Facing::DOWN;
		loadAnimation(jumpDown);
		jumpByY = moveDownSpeed*JUMP_TIME*speedFactor;
		endHalfJumpPosition = position + Vector3(0, jumpByY, MAX_JUMP_HEIGHT);
	} else {
		// no direction input

		switch (facing) {
			case Facing::LEFT:
				loadAnimation(jumpLeft);
				jumpByX = -moveRightSpeed * JUMP_TIME * speedFactor;
				endHalfJumpPosition = position + Vector3(jumpByX, 0, MAX_JUMP_HEIGHT);
				break;
			case Facing::DOWN:
				// moving down
				loadAnimation(jumpDown);
				jumpByY = moveDownSpeed * JUMP_TIME * speedFactor;
				endHalfJumpPosition = position + Vector3(0, jumpByY, MAX_JUMP_HEIGHT);
				break;
			case Facing::RIGHT:
				loadAnimation(jumpRight);
				jumpByX = moveRightSpeed * JUMP_TIME * speedFactor;
				endHalfJumpPosition = position + Vector3(jumpByX, 0, MAX_JUMP_HEIGHT);
				break;
			case Facing::UP:
				// moving up
				loadAnimation(jumpUp);
				jumpByY = -moveDownSpeed * JUMP_TIME * speedFactor;
				endHalfJumpPosition = position + Vector3(0, jumpByY, MAX_JUMP_HEIGHT);
				break;
		}
	}


	canCancelAction = false;
	action = CreatureAction::JUMP_ACTION;
	moving = false;
	jumpTime = 0;
	startJumpPosition = position;
	jumpingRising = true;
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


void PlayerCharacter::jumpUpdate(double deltaTime) {

	jumpTime += deltaTime;

	double percentJumped = jumpTime / JUMP_TIME;
	setPosition(
		Vector3::Lerp(startJumpPosition, endHalfJumpPosition, percentJumped));
	if (percentJumped >= 1) {
		if (jumpingRising) {
			jumpingRising = false;
			jumpTime = 0;
			startJumpPosition = endHalfJumpPosition;
			endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, -MAX_JUMP_HEIGHT);
		} else {
			double percentJumped = jumpTime / JUMP_TIME;
			setPosition(
				Vector3::Lerp(startJumpPosition, endHalfJumpPosition, percentJumped));
			if (percentJumped >= 1) {
				// landed
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
				canCancelAction = true;
				moving = false;
			}
		}
	}


}


void PlayerCharacter::movement(double deltaTime) {

	Vector3 moveVector = getMovement(deltaTime, joystick->lAxisX, joystick->lAxisY);
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
		} else if (!collision) {
			moveBy(moveVector);
		}

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
		speedFactor = RUN_SPEED;
	else
		speedFactor = NORMAL_SPEED;

	Vector3 moveVector = Vector3::Zero;
	if (horzDirection > 10) {
		// moving right
		if (vertDirection < -10) {
			// moving right & up
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			moveVector = Vector3(moveByX, -moveByY, 0);
		} else if (vertDirection > 10) {
			// moving right & down
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			moveVector = Vector3(moveByX, moveByY, 0);
		} else {
			float moveByX = moveRightSpeed*deltaTime*speedFactor;
			moveVector = Vector3(moveByX, 0, 0);
		}

		if (!moving || facing != Facing::RIGHT) {
			if (runningNow)
				loadAnimation("run right");
			else
				loadAnimation(walkRight);
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
			moveVector = Vector3(-moveByX, -moveByY, 0);
		} else if (vertDirection > 10) {
			// moving left & down
			float moveByX = moveDiagonalRight*deltaTime*speedFactor;
			float moveByY = moveDiagonalDown*deltaTime*speedFactor;
			moveVector = Vector3(-moveByX, moveByY, 0);
		} else {
			float moveByX = moveRightSpeed*deltaTime*speedFactor;
			moveVector = Vector3(-moveByX, 0, 0);
		}

		if (!moving || facing != Facing::LEFT) {
			if (runningNow)
				loadAnimation("run left");
			else
				loadAnimation(walkLeft);
			moving = true;
			facing = Facing::LEFT;
		}

		waiting = false;
		return moveVector;
	}


	if (vertDirection < -10) {
		// moving up
		float moveByY = moveDownSpeed*deltaTime*speedFactor;
		moveVector = Vector3(0, -moveByY, 0);
		if (!moving || facing != Facing::UP) {
			if (runningNow)
				loadAnimation("run up");
			else
				loadAnimation(walkUp);
			moving = true;
			facing = Facing::UP;
		}
		waiting = false;
		return moveVector;
	}

	if (vertDirection > 10) {
		// moving down
		float moveByY = moveDownSpeed*deltaTime*speedFactor;
		moveVector = Vector3(0, moveByY, 0);
		if (!moving || facing != Facing::DOWN) {
			if (runningNow)
				loadAnimation("run down");
			else
				loadAnimation(walkDown);
			moving = true;
			facing = Facing::DOWN;
		}
		waiting = false;
		return moveVector;
	}

	return moveVector;
}