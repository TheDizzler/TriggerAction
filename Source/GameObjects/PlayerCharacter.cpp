#include "../pch.h"
#include "PlayerCharacter.h"


PlayerCharacter::PlayerCharacter(shared_ptr<Joystick> joy) {
	joystick = joy;
	joystick->pc = this;
}

PlayerCharacter::~PlayerCharacter() {
}

#include "../Managers/MapManager.h"
void PlayerCharacter::initialize(const CharacterData* characterData, int startingPosition) {

	playerNumber = startingPosition;
	name = characterData->name;
	assetSet = characterData->assets;
	hitbox = make_unique<Hitbox>(characterData->hitbox.get());

	
	loadAnimation("stand right");
	origin = Vector2(0, getHeight());
	

}

void PlayerCharacter::setInitialPosition() {

	setPosition(Vector3(100, 64 * playerNumber, 0));

}


#include "../Screens/LevelScreen.h"
void PlayerCharacter::update(double deltaTime) {

	if (getMovement(deltaTime, joystick->lAxisX, joystick->lAxisY)) {

		// check for collisions
		for (const Hitbox* hb : hitboxesAll) {
			if (hb == hitbox.get())
				continue;
			if (hitbox->collision2d(hb)) {
				int  hit = 69;
			}
		}
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

	batch->Draw(currentAnimation->texture.Get(), /*drawPosition*/ position,
		&currentAnimation->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		origin, scale, spriteEffects, layerDepth);

	debugDraw(batch);
}


#include <math.h>
bool PlayerCharacter::getMovement(double deltaTime, int horzDirection, int vertDirection) {

	if (horzDirection > 10) {
		// moving right

		if (vertDirection < -10) {
			// moving right & up

			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			moveBy(Vector3(moveByX, -moveByY, 0));
			layerDepth = Map::getLayerDepth(position.y);
		} else if (vertDirection > 10) {
			// moving right & down
			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			moveBy(Vector3(moveByX, moveByY, 0));
			layerDepth = Map::getLayerDepth(position.y);
		/*	wostringstream wss;
			wss << "moveRightSpeed: " << moveRightSpeed << " moveDownSpeed: " << moveDownSpeed << endl;
			wss << "moveDiagonalRight: " << moveDiagonalRight << " moveDiagonalDown: " << moveDiagonalDown << endl;
			OutputDebugString(wss.str().c_str());*/
		} else {
			float moveByX = moveRightSpeed*deltaTime;
			moveBy(Vector3(moveByX, 0, 0));

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
			moveBy(Vector3(-moveByX, -moveByY, 0));
			layerDepth = Map::getLayerDepth(position.y);
		} else if (vertDirection > 10) {
			// moving left & down
			float moveByX = moveDiagonalRight*deltaTime;
			float moveByY = moveDiagonalDown*deltaTime;
			moveBy(Vector3(-moveByX, moveByY, 0));
			layerDepth = Map::getLayerDepth(position.y);
		} else {
			float moveByX = moveRightSpeed*deltaTime;
			moveBy(Vector3(-moveByX, 0, 0));

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
		moveBy(Vector3(0, -moveByY, 0));
		layerDepth = Map::getLayerDepth(position.y);
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
		moveBy(Vector3(0, moveByY, 0));
		layerDepth = Map::getLayerDepth(position.y);
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
	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;

	drawPosition.x = position.x;
	drawPosition.y = position.y - currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
}

bool PlayerCharacter::checkCollisionWith(const Hitbox * hitbox) const {
	return false;
}

int PlayerCharacter::getHeight() const {
	return currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
}

const Hitbox* PlayerCharacter::getHitbox() const {
	return hitbox.get();
}

void PlayerCharacter::moveBy(const Vector3& moveVector) {

	position += moveVector;
	drawPosition.x += moveVector.x;
	drawPosition.y += moveVector.y - position.z;

	hitbox->position += moveVector;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveVector;

	//layerDepth = Map::getLayerDepth(position.y);

	debugUpdate(Vector2(moveVector.x, moveVector.y));
}

void PlayerCharacter::setPosition(const Vector3& newpos) {

	Vector3 moveBy = newpos - position;
	position = newpos;
	drawPosition.x = position.x;
	drawPosition.y = position.y - position.z;

	hitbox->position += moveBy;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveBy;

	layerDepth = Map::getLayerDepth(position.y);
}


