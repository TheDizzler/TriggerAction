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



void PlayerCharacter::update(double deltaTime) {

	bool moved = getMovement(deltaTime, joystick->lAxisX, joystick->lAxisY);
	//if (joystick->lAxisY < -10) {
	//	// moving up
	//	position.y -= moveDownSpeed*deltaTime;
	//	if (!moving || facing != Facing::UP) {
	//		loadAnimation("walk up");
	//		moving = true;
	//		facing = Facing::UP;
	//	}
	//	moved = true;
	//	waiting = false;
	//} else if (joystick->lAxisY > 10) {
	//	// moving down
	//	position.y += moveDownSpeed*deltaTime;
	//	if (!moving || facing != Facing::DOWN) {
	//		loadAnimation("walk down");
	//		moving = true;
	//		facing = Facing::DOWN;
	//	}
	//	moved = true;
	//	waiting = false;
	//}

	//if (joystick->lAxisX < -10) {
	//	// moving left
	//	position.x -= moveRightSpeed*deltaTime;
	//	if (!moving || facing != Facing::LEFT) {
	//		loadAnimation("walk right");
	//		moving = true;
	//		facing = Facing::LEFT;
	//	}
	//	spriteEffects = SpriteEffects_FlipHorizontally;
	//	moved = true;
	//	waiting = false;
	//} else if (joystick->lAxisX > 10) {
	//	// moving right
	//	position.x += moveRightSpeed*deltaTime;
	//	if (!moving || facing != Facing::RIGHT) {
	//		loadAnimation("walk right");
	//		moving = true;
	//		facing = Facing::RIGHT;
	//	}
	//	spriteEffects = SpriteEffects_None;
	//	moved = true;
	//	waiting = false;
	//}

	if (!moved && !waiting) {
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
	}
}


#include <math.h>
bool PlayerCharacter::getMovement(double deltaTime, int horzDirection, int vertDirection) {

	if (horzDirection > 10) {
		// moving right

		if (vertDirection < -10) {
			// moving right & up
			position.x += moveDiagonalRight*deltaTime;
			position.y -= moveDiagonalDown*deltaTime;
		} else if (vertDirection > 10) {
			// moving right & down
			position.x += moveDiagonalRight*deltaTime;
			position.y += moveDiagonalDown*deltaTime;
		/*	wostringstream wss;
			wss << "moveRightSpeed: " << moveRightSpeed << " moveDownSpeed: " << moveDownSpeed << endl;
			wss << "moveDiagonalRight: " << moveDiagonalRight << " moveDiagonalDown: " << moveDiagonalDown << endl;
			OutputDebugString(wss.str().c_str());*/
		} else {
			position.x += moveRightSpeed*deltaTime;
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
			position.x -= moveDiagonalRight*deltaTime;
			position.y -= moveDiagonalDown*deltaTime;
		} else if (vertDirection > 10) {
			// moving left & down
			position.x -= moveDiagonalRight*deltaTime;
			position.y += moveDiagonalDown*deltaTime;
		} else {
			position.x -= moveRightSpeed*deltaTime;
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
		position.y -= moveDownSpeed*deltaTime;
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
		position.y += moveDownSpeed*deltaTime;
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
}


void PlayerCharacter::draw(SpriteBatch* batch) {

	batch->Draw(currentAnimation->texture.Get(), position,
		&currentAnimation->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		origin, scale, spriteEffects, layerDepth);
}