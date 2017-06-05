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


	setHitbox(characterData->hitbox.get());
	radarBox = Hitbox(hitbox);



}

PlayerCharacter::~PlayerCharacter() {
}

void PlayerCharacter::reloadData(CharacterData* data) {

	characterData = data;
	initializeAssets();
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
				if (joystick->xButton()) {
					startJump();
				} else if (joystick->bButtonStates[ControlButtons::L]) {
					startBlock();
				} else {
					int horzDirection = joystick->lAxisX;
					int vertDirection = joystick->lAxisY;

					if (horzDirection > 10 || horzDirection < -10
						|| vertDirection < -10 || vertDirection > 10) {
						movement(deltaTime/*, horzDirection, vertDirection*/);
					}
				}
			}
			break;
		case CreatureAction::WAITING_ACTION:
		default:
			waitUpdate(deltaTime);
			if (joystick->yButton()) {
				startMainAttack();
				//startDrawWeapon();
			} else if (joystick->xButton()) {
				startJump();
			} else if (joystick->bButtonStates[ControlButtons::L]) {
				startBlock();
			} else {
				int horzDirection = joystick->lAxisX;
				int vertDirection = joystick->lAxisY;
				if (horzDirection > 10 || horzDirection < -10
					|| vertDirection < -10 || vertDirection > 10) {
					movement(deltaTime/*, horzDirection, vertDirection*/);

				}
				//moveUpdate(deltaTime);
			}
			break;
		case CreatureAction::MOVING_ACTION:
			if (joystick->yButton()) {
				startMainAttack();
				//startDrawWeapon();
			} else if (joystick->xButton()) {
				startJump();
			} else if (joystick->bButtonStates[ControlButtons::L]) {
				startBlock();
			} else {
				/*int horzDirection = joystick->lAxisX;
				int vertDirection = joystick->lAxisY;
				if (horzDirection > 10 || horzDirection < -10
					|| vertDirection < -10 || vertDirection > 10) {*/
					movement(deltaTime/*, horzDirection, vertDirection*/);

				//} 
				moveUpdate(deltaTime);
			}

			break;

		case CreatureAction::BLOCK_ACTION:
			blockUpdate(deltaTime);
			break;
		case CreatureAction::JUMP_ACTION:

			jumpUpdate(deltaTime);
			break;
		case CreatureAction::HIT_ACTION:
			hitUpdate(deltaTime);
			break;
		case CreatureAction::DRAWING_ACTION:
			drawWeaponUpdate(deltaTime);
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

void PlayerCharacter::takeDamage(int damage) {

	if ((hp -= damage) < 0) {
		hp = 0;
		isAlive = false;
		timeSinceDeath = 0;
	}

	action = CreatureAction::HIT_ACTION;
	canCancelAction = false;
	loadAnimation(hit);
}


void PlayerCharacter::initializeAssets() {
	assetSet = characterData->assets;

	standDown = assetSet->getAnimation("stand down");
	standLeft = assetSet->getAnimation("stand left");
	standUp = assetSet->getAnimation("stand up");
	standRight = assetSet->getAnimation("stand right");

	runDown = assetSet->getAnimation("run down");
	runLeft = assetSet->getAnimation("run left");
	runUp = assetSet->getAnimation("run up");
	runRight = assetSet->getAnimation("run right");

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

	combatStanceDown = assetSet->getAnimation("combat stance down");
	combatStanceLeft = assetSet->getAnimation("combat stance left");
	combatStanceUp = assetSet->getAnimation("combat stance up");
	combatStanceRight = assetSet->getAnimation("combat stance right");

	provoke = assetSet->getAnimation("provoke");
	surprise = assetSet->getAnimation("surprise");
	hit = assetSet->getAnimation("hit");

	blockDown = assetSet->getAnimation("block down");
	blockLeft = assetSet->getAnimation("block left");
	blockUp = assetSet->getAnimation("block up");
	blockRight = assetSet->getAnimation("block right");

	shadow.load(assetSet->getAsset("shadow"));
}


void PlayerCharacter::startBlock() {

	action = CreatureAction::BLOCK_ACTION;

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
		case Facing::DOWN:
			loadAnimation(blockDown);
			break;
		case Facing::LEFT:
			loadAnimation(blockLeft);
			break;
		case Facing::UP:
			loadAnimation(blockUp);
			break;
		case Facing::RIGHT:
			loadAnimation(blockRight);
			break;
	}
}


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


void PlayerCharacter::startDrawWeapon() {

	action = CreatureAction::DRAWING_ACTION;
	canCancelAction = false;

	switch (facing) {
		case Facing::RIGHT:
			loadAnimation("draw weapon right");
			break;
		case Facing::LEFT:
			loadAnimation("draw weapon left");
			break;
		case Facing::DOWN:
			loadAnimation("draw weapon down");
			break;
		case Facing::UP:
			loadAnimation("draw weapon up");
			break;
	}

}


void PlayerCharacter::drawWeaponUpdate(double deltaTime) {

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
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


void PlayerCharacter::blockUpdate(double deltaTime) {

	if (!joystick->bButtonStates[ControlButtons::L]) {
		// end block
		action = CreatureAction::WAITING_ACTION;
		switch (facing) {
			case Facing::DOWN:
				loadAnimation(combatStanceDown);
				break;
			case Facing::LEFT:
				loadAnimation(combatStanceLeft);
				break;
			case Facing::UP:
				loadAnimation(combatStanceUp);
				break;
			case Facing::RIGHT:
				loadAnimation(combatStanceRight);
				break;
		}
		return;
	}

	int horzDirection = joystick->lAxisX;
	int vertDirection = joystick->lAxisY;

	if (horzDirection > 10) {
		// hop right
	} else if (horzDirection < -10) {
		// hop left
	} else if (vertDirection < -10) {
		// hop up
	} else if (vertDirection > 10) {
		// hop down
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
				moving = false;
			}
		}
	}


	int horzDirection = joystick->lAxisX;
	int vertDirection = joystick->lAxisY;
	if (horzDirection > 10) {
		facing = Facing::RIGHT;
		loadAnimation(jumpRight);
	} else if (horzDirection < -10) {
		facing = Facing::LEFT;
		loadAnimation(jumpLeft);

	} else if (vertDirection > 10) {
		facing = Facing::DOWN;
		loadAnimation(jumpDown);

	} else if (vertDirection < -10) {
		facing = Facing::UP;
		loadAnimation(jumpUp);

	}

}


void PlayerCharacter::movement(double deltaTime/*, int horzDirection, int vertDirection*/) {

	int horzDirection = joystick->lAxisX;
	int vertDirection = joystick->lAxisY;
	Vector3 moveVector = getMovement(deltaTime, horzDirection, vertDirection);
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
				loadAnimation(runRight);
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
				loadAnimation(runLeft);
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
				loadAnimation(runUp);
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
				loadAnimation(runDown);
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



