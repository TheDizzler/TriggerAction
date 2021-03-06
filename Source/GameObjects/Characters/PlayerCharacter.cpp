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
	radarBox = Hitbox(hitbox);
	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	currentHP = maxHP;
	currentMP = maxMP;
	playerSlot->statusDialog->updateHP();
	isAlive = true;

	canCancelAction = true;
	action = CreatureAction::WAITING_ACTION;

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


void PlayerCharacter::setInitialPosition(const Vector3& startingPosition) {
	setPosition(startingPosition);
	loadAnimation(combatStanceRight);
	falling = true;
}


void PlayerCharacter::update(double deltaTime) {

	switch (action) {
		case CreatureAction::ATTACKING_ACTION:
			attackUpdate(deltaTime);
			if (canCancelAction) {
				if (joystick->xButtonPushed()) {
					startJump();
					position -= GRAVITY * (deltaTime + .0000001);
					position.z += LANDING_TOLERANCE;
				} else if (joystick->lButtonDown()) {
					startBlock();
				} else {
					//int horzDirection = joystick->lAxisX;
					//int vertDirection = joystick->lAxisY;

					/*if (horzDirection > 10 || horzDirection < -10
						|| vertDirection < -10 || vertDirection > 10) {*/
					if (joystick->getDirection() != Vector2::Zero) {
						movement(deltaTime);
					}
				}
			}

			break;
		case CreatureAction::WAITING_ACTION:
		default:
			waitUpdate(deltaTime);
			if (joystick->yButtonPushed()) {
				startMainAttack();
			} else if (joystick->xButtonPushed()) {
				startJump();
				position -= GRAVITY * (deltaTime + .0000001);
				position.z += LANDING_TOLERANCE;
			} else if (joystick->lButtonDown()) {
				startBlock();
			} else {
				/*int horzDirection = joystick->lAxisX;
				int vertDirection = joystick->lAxisY;
				if (horzDirection > 10 || horzDirection < -10
					|| vertDirection < -10 || vertDirection > 10) {*/
				if (joystick->getDirection() != Vector2::Zero) {
					movement(deltaTime);
				}
			}

			break;
		case CreatureAction::MOVING_ACTION:
			if (joystick->yButtonPushed()) {
				startMainAttack();
			} else if (joystick->xButtonPushed()) {
				startJump();
				position -= GRAVITY * (deltaTime + .0000001);
				position.z += LANDING_TOLERANCE;
			} else if (joystick->lButtonDown()) {
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
			timeSinceDeath += deltaTime;
			if (moveVelocity != Vector3::Zero) {

				if (!falling) {
					moveVelocity = moveVelocity * GROUND_FRICTION;
					if (abs(moveVelocity.x) <= 1 && abs(moveVelocity.y) <= 1) {
						moveVelocity = Vector3::Zero;
						guiOverlay->openCharacterSelectDialog(playerSlot.get());
						isAlive = false;
					}
				}
			}

			break;
	}

	if (falling) {

		Vector3 moveVector = moveVelocity * deltaTime + fallVelocity;
		descending = abs(fallVelocity.z) > moveVector.z;

		if (position.z <= 0) {
			Vector3 newpos = position;
			newpos.z = 0;
			setPosition(newpos);
			fallVelocity.z = 0;
			moveVelocity.z = 0;
			moveVector.z = 0;
			falling = false;
		} else {
			radarBox.position = hitbox.position + moveVector;
			// check for collisions
			for (Tangible* tangible : tangiblesAll) {
				if (tangible == this)
					continue;
				if (radarBox.collision2d(tangible->getHitbox())) {
				// first check to see if hitbox overlap on x-y plane
					if (radarBox.collisionZ(tangible->getHitbox())) {
						// then check if collide on z-axis as well


						if (descending) {
							const Hitbox* hb = tangible->getHitbox();
							float dif = position.z - (hb->position.z + hb->size.z);
							if (dif < LANDING_TOLERANCE
								&& dif > -LANDING_TOLERANCE) {
								// land on that platform!
								Vector3 newpos = position;
								newpos.z = hb->position.z + hb->size.z;
								setPosition(newpos);
								fallVelocity.z = 0;
								moveVelocity.z = 0;
								moveVector.z = 0;
								falling = false;
								break;
							}
						} else {
							//float dif = (radarBox.position.z + radarBox.size.z)
							//	- (hb->position.z);
							//if (dif >= 0) {
							//// hit your head!
							//	Vector3 newpos = position;
							//	newpos.z -= dif;
							//	setPosition(newpos);
							fallVelocity.z = 0;
							moveVelocity.z = 0;
							moveVector.z = 0;
							descending = true;
						//}
						}
					} else {
						for (const auto& otherSubHB : tangible->subHitboxes) {
							if (otherSubHB->collision(&radarBox)) {
								const Hitbox* hb = otherSubHB.get();
								if (descending) {
									float dif = position.z - (hb->position.z + hb->size.z);
									if (dif < LANDING_TOLERANCE
										&& dif > -LANDING_TOLERANCE) {
										// land on that platform!
										Vector3 newpos = position;
										newpos.z = hb->position.z + hb->size.z;
										setPosition(newpos);
										fallVelocity.z = 0;
										moveVelocity.z = 0;
										moveVector.z = 0;
										falling = false;
										break;
									}
								} else {
									float dif = (radarBox.position.z + radarBox.size.z)
										- (hb->position.z);
									if (dif >= 0) {
										// hit your head!
										Vector3 newpos = position;
										newpos.z = hb->position.z - radarBox.size.z;
										setPosition(newpos);
										fallVelocity.z = 0;
										moveVelocity.z = 0;
										moveVector.z = 0;
									}
								}
								/*float dif = position.z - (hb->position.z + hb->size.z);

								if (dif < LANDING_TOLERANCE
									&& dif > -LANDING_TOLERANCE
									&& descending) {
									Vector3 newpos = position;
									newpos.z = hb->position.z + hb->size.z;
									setPosition(newpos);
									fallVelocity.z = 0;
									moveVelocity.z = 0;
									moveVector.z = 0;
									falling = false;
									break;
								}*/
							}
						}
					}
				}
			}
		}

		for (Trigger* trigger : triggersAll) {
			radarBox.position = hitbox.position + moveVector * 2;
			if (checkCollisionWith(trigger)) {
				if (trigger->activateTrigger(this)) {
					//moveVector = moveVelocity * deltaTime + fallVelocity; ??? only for stairs!
					break;
				}
			}
		}

		if (!(moveVelocity.x == 0 && moveVelocity.y == 0)) {

			moveVector = collisionMovement(moveVector);

		}

		moveBy(moveVector);
		//if (falling)
		fallVelocity += GRAVITY * deltaTime;
	} else if (!(moveVelocity.x == 0 && moveVelocity.y == 0)) {

		moveVelocity *= GROUND_FRICTION;
		if (abs(moveVelocity.x) <= 1 && abs(moveVelocity.y) <= 1) {
			moveVelocity = Vector3::Zero;
		} else {
			Vector3 moveVector = moveVelocity * deltaTime;

			for (Trigger* trigger : triggersAll) {
				radarBox.position = hitbox.position + moveVector * 2;
				if (checkCollisionWith(trigger)) {
					if (trigger->activateTrigger(this))
						break;
				}
			}
			moveBy(collisionMovement(moveVelocity * deltaTime));

		}
		// check if falling
		if (position.z > 0) {
			falling = true;
		}
	}


	if (joystick->startButtonPushed()) {

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

	if (action == CreatureAction::HIT_ACTION || action == CreatureAction::DEAD_ACTION) {
		// prevents further damage while recovering from hit
		return;
	}

	if (action == CreatureAction::BLOCK_ACTION) {
		damage *= .25;
		moveVelocity *= .5; // dampen knockback
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

		action = CreatureAction::HIT_ACTION;
	}



	if ((currentHP -= damage) <= 0) {
		currentHP = 0;
		action = CreatureAction::DEAD_ACTION;
		switch (facing) {
			case Facing::LEFT:
				loadAnimation("dead left");
				break;
			case Facing::RIGHT:
				loadAnimation("dead right");
				break;
		}
		remove(tangiblesAll.begin(), tangiblesAll.end(), this);
		timeSinceDeath = 0;
	}

	if (showDamage) {
		Vector3 jampos = position;
		jampos.x -= getWidth() / 2;
		jampos.y -= getHeight() / 2;
		LevelScreen::jammerMan.createJam(jampos, damage);
	}

	playerSlot->statusDialog->updateHP();

	//if (position.z != 0) {
	//	// start falling!
	//	falling = true;

	//}

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


void PlayerCharacter::startBlock() {

	action = CreatureAction::BLOCK_ACTION;
	moving = false;
	running = false;
	//moveVelocity.x = 0;
	moveVelocity.y = 0;
	//int horzDirection = joystick->lAxisX;
	//int vertDirection = joystick->lAxisY;

	/*if (horzDirection > 10) {
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

void PlayerCharacter::blockUpdate(double deltaTime) {

	//if (!joystick->bButtonStates[ControlButtons::L]) {
	if (!joystick->lButtonDown()) {
		// end block
		action = CreatureAction::WAITING_ACTION;
		moving = false;
		running = false;
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

	//int horzDirection = joystick->lAxisX;
	//int vertDirection = joystick->lAxisY;

	//if (horzDirection > 10) {
	//	// hop right
	//} else if (horzDirection < -10) {
	//	// hop left
	//} else if (vertDirection < -10) {
	//	// hop up
	//} else if (vertDirection > 10) {
	//	// hop down
	//}
}


void PlayerCharacter::startJump() {

	Vector3 direction;

	//
	//	facing = Facing::RIGHT;
	//} else if (joystick->isLeftPressed()) {
	//	facing = Facing::LEFT;
	//} else if (joystick->isUpPressed()) {
	//	facing = Facing::UP;
	//} else if (joystick->isDownPressed()) {
	//	facing = Facing::DOWN;
	//}

	/*int horzDirection = joystick->lAxisX;
	int vertDirection = joystick->lAxisY;
	if (horzDirection > 10) {*/
	if (joystick->isRightPressed()) {
		// moving right
		facing = Facing::RIGHT;
		loadAnimation(jumpRight);
		//if (vertDirection < -10) {
		if (joystick->isUpPressed()) {
				// moving right & up
			direction = Vector3(moveDiagonalRight, -moveDiagonalDown, 0);
		//} else if (vertDirection > 10) {
		} else if (joystick->isDownPressed()) {
				// moving right & down
			direction = Vector3(moveDiagonalRight, moveDiagonalDown, 0);
		} else {
			// moving right
			direction = Vector3(moveRightSpeed, 0, 0);
		}
	//} else if (horzDirection < -10) {
	} else if (joystick->isLeftPressed()) {
		// moving left
		loadAnimation(jumpLeft);
		facing = Facing::LEFT;
		//if (vertDirection < -10) {
		if (joystick->isUpPressed()) {
			// moving left & up
			direction = Vector3(-moveDiagonalRight, -moveDiagonalDown, 0);
		//} else if (vertDirection > 10) {
		} else if (joystick->isDownPressed()) {
			// moving left & down
			direction = Vector3(-moveDiagonalRight, moveDiagonalDown, 0);
		} else {
			// moving left
			direction = Vector3(-moveRightSpeed, 0, 0);
		}
	//} else if (vertDirection < -10) {
	} else if (joystick->isUpPressed()) {
		// moving up
		facing = Facing::UP;
		loadAnimation(jumpUp);
		direction = Vector3(0, -moveDownSpeed, 0);
	//} else if (vertDirection > 10) {
	} else if (joystick->isDownPressed()) {
		// moving down
		facing = Facing::DOWN;
		loadAnimation(jumpDown);
		direction = Vector3(0, moveDownSpeed, 0);
	} else {
		// no direction input
		switch (facing) {
			case Facing::LEFT:
				loadAnimation(jumpLeft);
				break;
			case Facing::DOWN:
				// moving down
				loadAnimation(jumpDown);
				break;
			case Facing::RIGHT:
				loadAnimation(jumpRight);
				break;
			case Facing::UP:
				// moving up
				loadAnimation(jumpUp);
				break;
		}
	}

	float speedFactor;

	if (running) {
		speedFactor = RUN_SPEED;
		direction.z = 300;
	} else {
		speedFactor = NORMAL_SPEED;
		direction.z = 250;
	}

	moveVelocity = Vector3(direction.x * speedFactor, direction.y * speedFactor, direction.z);


	canCancelAction = false;
	action = CreatureAction::JUMP_ACTION;
	moving = false;
	falling = true;
}

void PlayerCharacter::jumpUpdate(double deltaTime) {


	if (!falling) {
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
		moveVelocity = Vector3::Zero;
		canCancelAction = true;
		moving = false;
		running = false;
	} else {

		/*int horzDirection = joystick->lAxisX;
		int vertDirection = joystick->lAxisY;*/

		Vector2 direction = joystick->getDirection();
		float speedFactor;
		if (running) {
			speedFactor = RUN_SPEED;
		} else {
			speedFactor = NORMAL_SPEED;
		}

		/* High control jump. */
		/*if (horzDirection != 0 && vertDirection != 0) {
			moveVelocity.x = moveDiagonalRight * speedFactor * horzDirection / 127;
			moveVelocity.y = moveDiagonalDown * speedFactor * vertDirection / 127;
		} else {
			moveVelocity.x = moveRightSpeed * speedFactor * horzDirection / 127;
			moveVelocity.y = moveDownSpeed * speedFactor * vertDirection / 127;
		}*/

		if (direction != Vector2::Zero) {
			moveVelocity.x = moveDiagonalRight * speedFactor * direction.x;
			moveVelocity.y = moveDiagonalDown * speedFactor * direction.y;
		} else {
			moveVelocity.x = moveRightSpeed * speedFactor * direction.x;
			moveVelocity.y = moveDownSpeed * speedFactor * direction.y;
		}


		/* Low-control hump. */
		//moveVelocity.x += horzDirection * 2 * deltaTime;
		//moveVelocity.y += vertDirection * 2 * deltaTime;

		//Vector3 moveVector = moveVelocity * deltaTime;

		//radarBox.position = hitbox.position + moveVector * 2;
		////bool collision = false;
		//// check for collisions
		//for (Tangible* hb : tangiblesAll) {
		//	if (hb == this)
		//		continue;
		//	if (checkCollisionWith(hb)) {
		//		/*collision = true;*/ // it's POSSIBLE that more than one object could collide
		//		moveVelocity.x = 0;
		//		moveVelocity.y = 0;
		//		//moveVector.x = 0;
		//		//moveVector.y = 0;
		//		break;
		//	}
		//}



		/*if (horzDirection > 10) {
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
		}*/

		if (joystick->isRightPressed()) {
			facing = Facing::RIGHT;
			loadAnimation(jumpRight);
		} else if (joystick->isLeftPressed()) {
			facing = Facing::LEFT;
			loadAnimation(jumpLeft);
		} else if (joystick->isUpPressed()) {
			facing = Facing::UP;
			loadAnimation(jumpDown);
		} else if (joystick->isDownPressed()) {
			facing = Facing::DOWN;
			loadAnimation(jumpUp);
		}
	}
}


void PlayerCharacter::hitUpdate(double deltaTime) {

	if (!falling) {
		if (moveVelocity == Vector3::Zero) {
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

Vector3 PlayerCharacter::collisionMovement(Vector3& moveVector) {

	radarBox.position = hitbox.position + moveVector * 2;

	// check for collisions
	bool collision = false;
	vector<Tangible*> collided;
	for (Tangible* tangible : tangiblesAll) {
		if (tangible == this)
			continue;
		if (checkCollisionWith(tangible)) {
			if (moveVelocity.x == 0) {
				moveVelocity.y = 0;
				moveVector.y = 0;
				break;
			} else if (moveVelocity.y == 0) {
				moveVelocity.x = 0;
				moveVector.x = 0;
				break;
			}
			collision = true;
			collided.push_back(tangible);
		}
	}

	if (collision) {
		collision = false;
		Vector3 backupVector = moveVector;
		moveVector.x = 0;
		radarBox.position = hitbox.position + moveVector * 2;

		for (const auto& tangible : collided) {
			if (checkCollisionWith(tangible)) {
				collision = true;
				break;
			}
		}


		if (!collision) {
			moveVelocity.x = 0;
		} else {
			moveVelocity.y = 0;
			moveVector.y = 0;
			moveVector.x = backupVector.x;

			radarBox.position = hitbox.position + moveVector * 2;

			for (const auto& tangible : collided) {
				if (checkCollisionWith(tangible)) {
					moveVelocity.x = 0;
					moveVector.x = 0;
					break;
				}
			}

		}
	}

	return moveVector;
}


void PlayerCharacter::movement(double deltaTime) {

	//int horzDirection = joystick->lAxisX;
	//int vertDirection = joystick->lAxisY;
	//Vector3 moveVector = getMovement(deltaTime, horzDirection, vertDirection);
	Vector2 dir = joystick->getDirection();
	Vector3 moveVector = getMovement(deltaTime, dir);
	if (moveVector == Vector3::Zero) {
		moveVelocity.x = 0;
		moveVelocity.y = 0;
		moving = false;
		running = false;
		action = CreatureAction::WAITING_ACTION;
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
	} else {
		action = CreatureAction::MOVING_ACTION;
		moveVelocity = moveVector;
	}
}


//Vector3 PlayerCharacter::getMovement(double deltaTime, int horzDirection, int vertDirection) {
Vector3 PlayerCharacter::getMovement(double deltaTime, Vector2 direction) {

	//bool runningNow = joystick->bButtonStates[ControlButtons::B];
	bool runningNow = joystick->bButtonDown();
	if (runningNow != running)
		moving = false;
	running = runningNow;
	float speedFactor;
	if (running)
		speedFactor = RUN_SPEED;
	else
		speedFactor = NORMAL_SPEED;

	Vector3 moveVector = Vector3::Zero;
	//if (horzDirection > 10) {
	if (direction.x > 0) {
		// moving right
		//if (vertDirection < -10) {
		if (direction.y < 0) {
			// moving right & up
			float moveByX = moveDiagonalRight/**deltaTime*/*speedFactor;
			float moveByY = moveDiagonalDown/**deltaTime*/*speedFactor;
			moveVector = Vector3(moveByX, -moveByY, 0);
		//} else if (vertDirection > 10) {
		} else if (direction.y > 0) {
			// moving right & down
			float moveByX = moveDiagonalRight/**deltaTime*/*speedFactor;
			float moveByY = moveDiagonalDown/**deltaTime*/*speedFactor;
			moveVector = Vector3(moveByX, moveByY, 0);
		} else {
			float moveByX = moveRightSpeed/**deltaTime*/*speedFactor;
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
		//waiting = false;
		return moveVector;
	}

	//if (horzDirection < -10) {
	if (direction.x < 0) {
		// moving left

		//if (vertDirection < -10) {
		if (direction.y < 0) {
			// moving left & up
			float moveByX = moveDiagonalRight/**deltaTime*/*speedFactor;
			float moveByY = moveDiagonalDown/**deltaTime*/*speedFactor;
			moveVector = Vector3(-moveByX, -moveByY, 0);
		//} else if (vertDirection > 10) {
		} else if (direction.y > 0) {
		// moving left & down
			float moveByX = moveDiagonalRight/**deltaTime*/*speedFactor;
			float moveByY = moveDiagonalDown/**deltaTime*/*speedFactor;
			moveVector = Vector3(-moveByX, moveByY, 0);
		} else {
			float moveByX = moveRightSpeed/**deltaTime*/*speedFactor;
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

		//waiting = false;
		return moveVector;
	}


	//if (vertDirection < -10) {
	if (direction.y < 0) {
		// moving up
		float moveByY = moveDownSpeed/**deltaTime*/*speedFactor;
		moveVector = Vector3(0, -moveByY, 0);
		if (!moving || facing != Facing::UP) {
			if (runningNow)
				loadAnimation(runUp);
			else
				loadAnimation(walkUp);
			moving = true;
			facing = Facing::UP;
		}
		//waiting = false;
		return moveVector;
	}

	//if (vertDirection > 10) {
	if (direction.y > 0) {
		// moving down
		float moveByY = moveDownSpeed/**deltaTime*/*speedFactor;
		moveVector = Vector3(0, moveByY, 0);
		if (!moving || facing != Facing::DOWN) {
			if (runningNow)
				loadAnimation(runDown);
			else
				loadAnimation(walkDown);
			moving = true;
			facing = Facing::DOWN;
		}
		//waiting = false;
		return moveVector;
	}

	return moveVector;
}



