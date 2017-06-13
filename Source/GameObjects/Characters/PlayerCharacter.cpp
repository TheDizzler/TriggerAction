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

	currentHP = maxHP;
	currentMP = maxMP;
	playerSlot->statusDialog->updateHP();
	isAlive = true;

	//setInitialPosition(Vector2(10, playerSlot->getPlayerSlotNumber() * 100 + 150));
	setInitialPosition(Vector2(180, 150));
	canCancelAction = true;
	action = CreatureAction::WAITING_ACTION;

}


void PlayerCharacter::setInitialPosition(const Vector2& startingPosition) {
	setPosition(Vector3(startingPosition.x, startingPosition.y, 20));
	loadAnimation(combatStanceRight);
	falling = true;
}


void PlayerCharacter::update(double deltaTime) {


	switch (action) {
		case CreatureAction::ATTACKING_ACTION:
			attackUpdate(deltaTime);
			if (canCancelAction) {
				if (joystick->xButton()) {
					startJump();
					position -= GRAVITY * (deltaTime + .0000001);
				} else if (joystick->bButtonStates[ControlButtons::L]) {
					startBlock();
				} else {
					int horzDirection = joystick->lAxisX;
					int vertDirection = joystick->lAxisY;

					if (horzDirection > 10 || horzDirection < -10
						|| vertDirection < -10 || vertDirection > 10) {
						movement(deltaTime);
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
				position -= GRAVITY * (deltaTime + .0000001);
			} else if (joystick->bButtonStates[ControlButtons::L]) {
				startBlock();
			} else {
				int horzDirection = joystick->lAxisX;
				int vertDirection = joystick->lAxisY;
				if (horzDirection > 10 || horzDirection < -10
					|| vertDirection < -10 || vertDirection > 10) {
					movement(deltaTime);
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
				position -= GRAVITY * (deltaTime + .0000001);
			} else if (joystick->bButtonStates[ControlButtons::L]) {
				startBlock();
			} else {
				movement(deltaTime);
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
		case CreatureAction::DEAD_ACTION:

			if (moveVelocity != Vector3::Zero) {
				timeSinceDeath += deltaTime;
				moveBy(moveVelocity * deltaTime);
				if (!falling) {
					moveVelocity = moveVelocity * GROUND_FRICTION;
					//moveVelocity.z = 0;
					if (abs(moveVelocity.x) <= 1 && abs(moveVelocity.y) <= 1) {
						moveVelocity = Vector3::Zero;
						guiOverlay->openCharacterSelectDialog(playerSlot.get());
						isAlive = false;
					}
				}
			}/* else {
				playerSlot->characterSelect(deltaTime);
			}*/
			break;

	}

	if (falling) {
		fallVelocity += GRAVITY * deltaTime;
		moveBy(fallVelocity);
		// get z-height of tile directly underneath
		/*vector<TileBase*> tiles = map->getTilesAt(position);
		TileBase* tileBelow = tiles[0];
		for (int i = 1; i < tiles.size(); ++i) {
			if (tiles[i]->getPosition().z < position.z
				&& tiles[i]->getPosition().z > tileBelow->getPosition().z) {
				tileBelow = tiles[i];
			}
		}*/
		if (position.z <= 0) {
			Vector3 newpos = position;
			newpos.z = 0;
			setPosition(newpos);
			fallVelocity.z = 0;
			moveVelocity.z = 0;
			falling = false;
		} else {
			radarBox.position = hitbox.position;
			// check for collisions
			for (const Tangible* tangible : hitboxesAll) {
				const Hitbox* hb = tangible->getHitbox();
				if (hb == &hitbox)
					continue;
				if (radarBox.collision2d(hb)) {
					if (position.z <= hb->position.z + hb->size.z) {
						Vector3 newpos = position;
						newpos.z = hb->position.z + hb->size.z;
						setPosition(newpos);
						fallVelocity.z = 0;
						moveVelocity.z = 0;
						falling = false;
					}
					//collision = true;
					//break;
				}
			}
		}

	}


	if (joystick->startButton()) {

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

void PlayerCharacter::takeDamage(int damage, bool showDamage) {

	if (action == CreatureAction::HIT_ACTION) {
		// prevents further damage while recovering from hit
		return;
	}

	if (action == CreatureAction::BLOCK_ACTION) {
		damage *= .25;
		moveVelocity *= .25; // dampen knockback
	} else {

		canCancelAction = false;

		switch (facing) {
			case Facing::DOWN:
				loadAnimation(hitDown);
				break;
			case Facing::LEFT:
				loadAnimation(hitLeft);
				break;
			case Facing::UP:
				loadAnimation(hitUp);
				break;
			case Facing::RIGHT:
				loadAnimation(hitRight);
				break;
		}
	}

	action = CreatureAction::HIT_ACTION;

	if ((currentHP -= damage) < 0) {
		currentHP = 0;
		/*isAlive = false;*/
		action = CreatureAction::DEAD_ACTION;
		switch (facing) {
			case Facing::LEFT:
				loadAnimation("dead left");
				break;
			case Facing::RIGHT:
				loadAnimation("dead right");
				break;
		}
		remove(hitboxesAll.begin(), hitboxesAll.end(), this);
		timeSinceDeath = 0;
	}

	if (showDamage)
		LevelScreen::jammerMan.createJam(position, damage);

	playerSlot->statusDialog->updateHP();

	if (position.z != 0) {
		// start falling!
		falling = true;

	}

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

	hitDown = assetSet->getAnimation("hit down");
	hitLeft = assetSet->getAnimation("hit left");
	hitUp = assetSet->getAnimation("hit up");
	hitRight = assetSet->getAnimation("hit right");

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

	Vector3 direction;


	int horzDirection = joystick->lAxisX;
	int vertDirection = joystick->lAxisY;
	if (horzDirection > 10) {
		// moving right
		facing = Facing::RIGHT;
		loadAnimation(jumpRight);
		if (vertDirection < -10) {
			// moving right & up
			//jumpByX = moveDiagonalRight*JUMP_TIME*speedFactor;
			//jumpByY = -moveDiagonalDown*JUMP_TIME*speedFactor;
			//endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, MAX_JUMP_HEIGHT);
			direction = Vector3(moveDiagonalRight, -moveDiagonalDown, 0);
		} else if (vertDirection > 10) {
			// moving right & down
			//jumpByX = moveDiagonalRight*JUMP_TIME*speedFactor;
			//jumpByY = moveDiagonalDown*JUMP_TIME*speedFactor;
			//endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, MAX_JUMP_HEIGHT);
			direction = Vector3(moveDiagonalRight, moveDiagonalDown, 0);
		} else {
			//jumpByX = moveRightSpeed*JUMP_TIME*speedFactor;
			//endHalfJumpPosition = position + Vector3(jumpByX, 0, MAX_JUMP_HEIGHT);
			direction = Vector3(moveRightSpeed, 0, 0);
		}
	} else if (horzDirection < -10) {
		// moving left
		loadAnimation(jumpLeft);
		facing = Facing::LEFT;
		if (vertDirection < -10) {
			// moving left & up
			//jumpByX = -moveDiagonalRight*JUMP_TIME*speedFactor;
			//jumpByY = -moveDiagonalDown*JUMP_TIME*speedFactor;
			//endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, MAX_JUMP_HEIGHT);
			direction = Vector3(-moveDiagonalRight, -moveDiagonalDown, 0);
		} else if (vertDirection > 10) {
			// moving left & down
			//jumpByX = -moveDiagonalRight/**JUMP_TIME*/*speedFactor;
			//jumpByY = moveDiagonalDown/**JUMP_TIME*/*speedFactor;
			//endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, MAX_JUMP_HEIGHT);
			direction = Vector3(-moveDiagonalRight, moveDiagonalDown, 0);
		} else {
			//jumpByX = -moveRightSpeed/**JUMP_TIME*/*speedFactor;
			//endHalfJumpPosition = position + Vector3(jumpByX, 0, MAX_JUMP_HEIGHT);
			direction = Vector3(-moveRightSpeed, 0, 0);
		}
	} else if (vertDirection < -10) {
		// moving up
		facing = Facing::UP;
		loadAnimation(jumpUp);
		//jumpByY = -moveDownSpeed/**JUMP_TIME*/*speedFactor;
		//endHalfJumpPosition = position + Vector3(0, jumpByY, MAX_JUMP_HEIGHT);
		direction = Vector3(0, -moveDownSpeed, 0);
	} else if (vertDirection > 10) {
		// moving down
		facing = Facing::DOWN;
		loadAnimation(jumpDown);
		//jumpByY = moveDownSpeed/**JUMP_TIME*/*speedFactor;
		//endHalfJumpPosition = position + Vector3(0, jumpByY, MAX_JUMP_HEIGHT);
		direction = Vector3(0, moveDownSpeed, 0);
	} else {
		// no direction input

		switch (facing) {
			case Facing::LEFT:
				loadAnimation(jumpLeft);
				//jumpByX = -moveRightSpeed /** JUMP_TIME*/ * speedFactor;
				//endHalfJumpPosition = position + Vector3(jumpByX, 0, MAX_JUMP_HEIGHT);
				direction = Vector3(-moveRightSpeed, 0, 0);
				break;
			case Facing::DOWN:
				// moving down
				loadAnimation(jumpDown);
				//jumpByY = moveDownSpeed/* * JUMP_TIME*/ * speedFactor;
				//endHalfJumpPosition = position + Vector3(0, jumpByY, MAX_JUMP_HEIGHT);
				direction = Vector3(0, moveDownSpeed, 0);
				break;
			case Facing::RIGHT:
				loadAnimation(jumpRight);
				//jumpByX = moveRightSpeed /** JUMP_TIME*/ * speedFactor;
				//endHalfJumpPosition = position + Vector3(jumpByX, 0, MAX_JUMP_HEIGHT);
				direction = Vector3(moveRightSpeed, 0, 0);
				break;
			case Facing::UP:
				// moving up
				loadAnimation(jumpUp);
				//jumpByY = -moveDownSpeed /** JUMP_TIME*/ * speedFactor;
				//endHalfJumpPosition = position + Vector3(0, jumpByY, MAX_JUMP_HEIGHT);
				direction = Vector3(0, -moveDownSpeed, 0);
				break;
		}
	}

	float speedFactor;
	if (running) {
		speedFactor = RUN_SPEED;
		direction.z = 355;
	} else {
		speedFactor = NORMAL_SPEED;
		direction.z = 275;
	}

	moveVelocity = Vector3(direction.x * speedFactor, direction.y * speedFactor, direction.z);
	//moveVelocity = Vector3(jumpByX, jumpByY, jumpZ * speedFactor);


	canCancelAction = false;
	action = CreatureAction::JUMP_ACTION;
	moving = false;
	//jumpTime = 0;
	//startJumpPosition = position;
	//jumpingRising = true;
	falling = true;
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
		moving = false;
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

	Vector3 moveVector = moveVelocity * deltaTime;
	if (!falling) {

		Vector3 newpos = moveVector + position;
		newpos.z = 0;
		setPosition(newpos);
		 //landed
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

	} else {

		radarBox.position = hitbox.position + moveVector * 2;
		bool collision = false;
		// check for collisions
		for (const Tangible* hb : hitboxesAll) {
			if (hb->getHitbox() == &hitbox)
				continue;
			if (radarBox.collision(hb->getHitbox())) {
				collision = true; // it's POSSIBLE that more than one object could collide
				moveVelocity.x = 0;
				moveVelocity.y = 0;
				break;
			}
		}


		moveBy(moveVector);

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
	/*jumpTime += deltaTime;
	double percentJumped = jumpTime / JUMP_TIME;
	setPosition(
		Vector3::Lerp(startJumpPosition, endHalfJumpPosition, percentJumped));*/
	//if (percentJumped >= 1) {
	//	if (jumpingRising) {
	//		jumpingRising = false;
	//		jumpTime = 0;
	//		startJumpPosition = endHalfJumpPosition;
	//		endHalfJumpPosition = position + Vector3(jumpByX, jumpByY, -MAX_JUMP_HEIGHT);
	//	} else {
	//		double percentJumped = jumpTime / JUMP_TIME;
	//		setPosition(
	//			Vector3::Lerp(startJumpPosition, endHalfJumpPosition, percentJumped));
	//		if (percentJumped >= 1) {
	//			// landed
	//			switch (facing) {
	//				case Facing::RIGHT:
	//					loadAnimation(combatStanceRight);
	//					break;
	//				case Facing::LEFT:
	//					loadAnimation(combatStanceLeft);
	//					break;
	//				case Facing::DOWN:
	//					loadAnimation(combatStanceDown);
	//					break;
	//				case Facing::UP:
	//					loadAnimation(combatStanceUp);
	//					break;
	//			}
	//			action = CreatureAction::WAITING_ACTION;
	//			canCancelAction = true;
	//			moving = false;
	//		}
	//	}
	//}
}


void PlayerCharacter::hitUpdate(double deltaTime) {

	moveBy(moveVelocity * deltaTime);
	if (!falling) {
		moveVelocity = moveVelocity * GROUND_FRICTION;
		//moveVelocity.z = 0;
		if (abs(moveVelocity.x) <= 1 && abs(moveVelocity.y) <= 1) {
			moveVelocity = Vector3::Zero;

			currentFrameTime += deltaTime;
			if (currentFrameTime >= currentFrameDuration) {
				if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
					// hit sequence done
					canCancelAction = true;
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
	}
}


void PlayerCharacter::movement(double deltaTime) {

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
			if (radarBox.collision(hb->getHitbox())) {
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



