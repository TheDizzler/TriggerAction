#include "../pch.h"
#include "PlayerCharacter.h"


PlayerCharacter::PlayerCharacter(shared_ptr<Joystick> joy) {
	joystick = joy;
}

PlayerCharacter::~PlayerCharacter() {
}

void PlayerCharacter::initialize(const shared_ptr<AssetSet> characterAssetSet, int startingPosition) {

	assetSet = characterAssetSet;


	loadAnimation("stand right");
	position = Vector2(100, 100 * startingPosition);
}


#include "../Managers/MapManager.h"
void PlayerCharacter::update(double deltaTime) {

	if (getMovement(deltaTime, joystick->lAxisX, joystick->lAxisY)) {

		//int size = currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
		//	- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
		layerDepth = Map::getLayerDepth(position.y);

	} else if (!waiting) {
		waiting = true;
		moving = false;
		switch (facing) {
			case Facing::RIGHT:
			case Facing::LEFT:
				loadAnimation("stand right");
				break;
			case Facing::DOWN:
				loadAnimation("stand down");
				break;
			case Facing::UP:
				loadAnimation("stand up");
				break;
		}
	}

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentAnimation->timePerFrame) {
		if (++currentFrameIndex >= currentAnimation->animationFrames.size())
			currentFrameIndex = 0;
		currentFrameTime = 0;
		drawPosition = position;
		drawPosition.y -= currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
			- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
	}
}


#include <math.h>
bool PlayerCharacter::getMovement(double deltaTime, int horzDirection, int vertDirection) {

	if (horzDirection > 10) {
		// moving right

		if (vertDirection < -10) {
			// moving right & up
			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			position.x += moveByX;
			position.y -= moveByY;
			drawPosition.x += moveByX;
			drawPosition.y += moveByY;
		} else if (vertDirection > 10) {
			// moving right & down
			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			position.x += moveByX;
			position.y += moveByY;
			drawPosition.x += moveByX;
			drawPosition.y += moveByY;
		/*	wostringstream wss;
			wss << "moveRightSpeed: " << moveRightSpeed << " moveDownSpeed: " << moveDownSpeed << endl;
			wss << "moveDiagonalRight: " << moveDiagonalRight << " moveDiagonalDown: " << moveDiagonalDown << endl;
			OutputDebugString(wss.str().c_str());*/
		} else {
			float moveByX = moveRightSpeed*deltaTime;
			position.x += moveByX;
			drawPosition.x += moveByX;
		}

		if (!moving || facing != Facing::RIGHT) {
			loadAnimation("walk right");
			moving = true;
			facing = Facing::RIGHT;
			spriteEffects = SpriteEffects_None;
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
			position.x -= moveByX;
			position.y -= moveByY;
			drawPosition.x -= moveByX;
			drawPosition.y -= moveByY;
		} else if (vertDirection > 10) {
			// moving left & down
			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			position.x -= moveByX;
			position.y += moveByY;
			drawPosition.x -= moveByX;
			drawPosition.y += moveByY;
		} else {
			float moveByX = moveRightSpeed*deltaTime;
			position.x -= moveByX;
			drawPosition.x -= moveByX;
		}

		if (!moving || facing != Facing::LEFT) {
			loadAnimation("walk right");
			moving = true;
			facing = Facing::LEFT;
			spriteEffects = SpriteEffects_FlipHorizontally;
		}

		waiting = false;
		return true;
	}


	if (vertDirection < -10) {
		// moving up
		float moveByY = moveDownSpeed*deltaTime;
		position.y -= moveByY;
		drawPosition.y -= moveByY;
		if (!moving || facing != Facing::UP) {
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
		position.y += moveByY;
		drawPosition.y += moveByY;
		if (!moving || facing != Facing::DOWN) {
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

	drawPosition = position;
	drawPosition.y -= currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
}


void PlayerCharacter::draw(SpriteBatch* batch) {

	batch->Draw(currentAnimation->texture.Get(), drawPosition,
		&currentAnimation->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		origin, scale, spriteEffects, layerDepth);
}