#include "../pch.h"
#include "PlayerCharacter.h"


PlayerCharacter::PlayerCharacter(shared_ptr<PlayerSlot> slot) {
	playerSlot = slot;
	joystick = playerSlot->getStick();

	characterData = slot->characterData;
	name = characterData->name;
	assetSet = characterData->assets;
	hitbox = make_unique<Hitbox>(characterData->hitbox.get());
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
			if (hb == hitbox.get())
				continue;
			if (hitbox->collision2d(hb)) {
				//debugSetTint(Color(1, .1, .1, 1));
				hit = true;
				int  hit = 69;
			}
		}

		if (hit && !lastHit)
			debugSetTint(Color(1, .1, .1, 1));
		else if (!hit && lastHit)
			debugSetTint(Color(1, 1, 1, 1));

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
		loadAnimation("shoot down");
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
		drawPosition.x = position.x;
		drawPosition.y = position.y - frame->sourceRect.bottom
			- frame->sourceRect.top;
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
	if (horzDirection > 10) {
		// moving right

		if (vertDirection < -10) {
			// moving right & up

			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			//moveBy(Vector3(moveByX, -moveByY, 0));
			layerDepth = Map::getLayerDepth(position.y);
		} else if (vertDirection > 10) {
			// moving right & down
			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			//moveBy(Vector3(moveByX, moveByY, 0));
			layerDepth = Map::getLayerDepth(position.y);
		} else {
			float moveByX = moveRightSpeed*deltaTime;
			//moveBy(Vector3(moveByX, 0, 0));

		}

		if (!moving || facing != Facing::RIGHT) {
			if (runningNow)
				loadAnimation("run right");
			else
				loadAnimation("walk right");
			moving = true;
			Vector3 hbpos = hitbox->position;
			hbpos.x += getWidth() / 2;
			setHitboxPosition(hbpos);
			facing = Facing::RIGHT;
			//spriteEffects = SpriteEffects_None;
		}
		waiting = false;
		return true;
	}

	if (horzDirection < -10) {
		// moving left

		if (vertDirection < -10) {
		// moving left & up
			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			//moveBy(Vector3(-moveByX, -moveByY, 0));
			layerDepth = Map::getLayerDepth(position.y);
		} else if (vertDirection > 10) {
			// moving left & down
			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			//moveBy(Vector3(-moveByX, moveByY, 0));
			layerDepth = Map::getLayerDepth(position.y);
		} else {
			float moveByX = moveRightSpeed*deltaTime;
			//moveBy(Vector3(-moveByX, 0, 0));

		}

		if (!moving || facing != Facing::LEFT) {
			if (runningNow)
				loadAnimation("run left");
			else
				loadAnimation("walk left");
			moving = true;
			facing = Facing::LEFT;
			Vector3 hbpos = Vector3::Zero;
			hbpos.x -= getWidth() / 2;
			moveHitboxBy(hbpos);
			//spriteEffects = SpriteEffects_FlipHorizontally;
		}

		waiting = false;
		return true;
	}


	if (vertDirection < -10) {
		// moving up
		float moveByY = moveDownSpeed*deltaTime;
		//moveBy(Vector3(0, -moveByY, 0));
		layerDepth = Map::getLayerDepth(position.y);
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
		float moveByY = moveDownSpeed*deltaTime;
		//moveBy(Vector3(0, moveByY, 0));
		layerDepth = Map::getLayerDepth(position.y);
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


void PlayerCharacter::loadAnimation(const pugi::char_t* name) {

	currentAnimation = assetSet->getAnimation(name);
	currentFrameIndex = 0;
	currentFrameTime = 0;
	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;

	drawPosition.x = position.x;
	drawPosition.y = position.y
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
}

bool PlayerCharacter::checkCollisionWith(const Hitbox* hitbox) const {
	return false;
}

int PlayerCharacter::getHeight() const {
	return currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
}

int PlayerCharacter::getWidth() const {
	return currentAnimation->animationFrames[currentFrameIndex]->sourceRect.right
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.left;
}

const Hitbox* PlayerCharacter::getHitbox() const {
	return hitbox.get();
}

void PlayerCharacter::moveBy(const Vector3& moveVector) {

	position += moveVector;
	drawPosition.x += moveVector.x;
	drawPosition.y += moveVector.y - position.z;

	hitbox->position += moveVector;
	//moveHitboxBy(moveVector);
	for (const auto& subHB : subHitboxes)
		subHB->position += moveVector;

	debugUpdate(Vector2(moveVector.x, moveVector.y));
}

void PlayerCharacter::setPosition(const Vector3& newpos) {

	Vector3 moveBy = newpos - position;
	position = newpos;
	drawPosition.x = position.x;
	drawPosition.y = position.y - position.z;

	hitbox->position += moveBy;
	//moveHitboxBy(moveBy);
	for (const auto& subHB : subHitboxes)
		subHB->position += moveBy;

	layerDepth = Map::getLayerDepth(position.y);
}


