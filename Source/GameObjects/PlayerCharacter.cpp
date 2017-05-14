#include "../pch.h"
#include "PlayerCharacter.h"


PlayerCharacter::PlayerCharacter(shared_ptr<PlayerSlot> slot) {
	playerSlot = slot;
	joystick = playerSlot->getStick();

	characterData = slot->characterData;
	name = characterData->name;
	assetSet = characterData->assets;
	setHitbox(characterData->hitbox.get());
	loadAnimation("stand right");
	origin = Vector2(0, getHeight());
}

PlayerCharacter::~PlayerCharacter() {
}

#include "../Managers/MapManager.h"
void PlayerCharacter::setInitialPosition(const Vector2& startingPosition) {

	setPosition(Vector3(startingPosition.x, startingPosition.y, 0));

}

#include "../GUIObjects/MenuDialog.h"
#include "../Screens/LevelScreen.h"
#include "../Engine/GameEngine.h"
void PlayerCharacter::update(double deltaTime) {

	if (getMovement(deltaTime, joystick->lAxisX, joystick->lAxisY)) {

		bool hit = false;
		// check for collisions
		for (const Hitbox* hb : hitboxesAll) {
			if (hb == &hitbox)
				continue;
			if (hitbox.collision2d(hb)) {
				hit = true;
				int  hit = 69;
			}
		}

		if (hit && !lastHit)
			debugSetTint(Color(1, .1, .1, 1));
		else if (!hit && lastHit)
			debugSetTint(Color(0, 0, 0, 1));

		lastHit = hit;

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

	if (joystick->bButtonStates[ControlButtons::Y]) {
		//loadAnimation("shoot down");
		startMainAttack();
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
}


void PlayerCharacter::draw(SpriteBatch* batch) {

	batch->Draw(currentAnimation->texture.Get(), position,
		&currentAnimation->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		currentAnimation->animationFrames[currentFrameIndex]->origin, scale,
		SpriteEffects_None, layerDepth);


	debugDraw(batch);
}


#include <math.h>
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

void PlayerCharacter::startMainAttack() {
	// get direction facing
	loadAnimation("shoot down");
}