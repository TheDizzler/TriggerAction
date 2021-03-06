#include "../pch.h"
#include "Creature.h"
#include "../Screens/LevelScreen.h"
#include "../Managers/MapManager.h"

Creature::Creature() {
	layerDepth = 0.1f;
}

Creature::~Creature() {
}

void Creature::loadMap(Map* mp) {
	map = mp;
}


void Creature::loadAnimation(Animation* animation) {
	currentAnimation = animation;
	currentFrameIndex = 0;
	currentFrameTime = 0;
	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect = currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin = currentAnimation->animationFrames[currentFrameIndex]->origin;
}

void Creature::loadAnimation(const pugi::char_t* name) {

	currentAnimation = assetSet->getAnimation(name);
	currentFrameIndex = 0;
	currentFrameTime = 0;
	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect = currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin = currentAnimation->animationFrames[currentFrameIndex]->origin;
}


void Creature::knockBack(Vector3 velocityOfHit, USHORT weightOfHit) {
	moveVelocity = velocityOfHit * weightOfHit / weight;
	falling = true;
	position -= GRAVITY * .00005f;
}

void Creature::knockBack(Vector3 hitVelocity) {
	moveVelocity = hitVelocity;
	falling = true;
	position -= GRAVITY * .00005f;
}

bool Creature::checkCollisionWith(Tangible* other) {

	if (radarBox.collision2d(other->getHitbox())) { // first check to see if hitbox overlap on x-y plane
		if (radarBox.collisionZ(other->getHitbox())) // then check if collide on z-axis as well
			return true;
		for (const auto& otherSubHB : other->subHitboxes) {
			if (otherSubHB->collision(&radarBox))
				return true;
		}
	}

	return false;
}

bool Creature::checkCollision2DWith(const Tangible* other) const {

	if (radarBox.collision2d(other->getHitbox())) { // first check to see if hitbox overlap on x-y plane
		if (radarBox.collisionZ(other->getHitbox())) // then check if collide on z-axis as well
			return true;
		for (const auto& otherSubHB : other->subHitboxes)
			if (otherSubHB->collision2d(&radarBox))
				return true;
	}

	return false;
}

const int Creature::getHeight() const {
	return currentFrameRect.bottom - currentFrameRect.top;
}

const int Creature::getWidth() const {
	return currentFrameRect.right - currentFrameRect.left;
}


void Creature::moveBy(const Vector3& moveVector) {
	IElement3D::moveBy(moveVector);
	moveHitboxBy(moveVector);
	shadow.moveBy(Vector2(moveVector.x, moveVector.y));

	layerDepth = Map::getLayerDepth(position.y);
	//falling == Map::standingOnGround(position);

	// bigger th z-coord smaller the shadow
	// if z = 0 then scale = 1
	// as z -> inf scale = 0
	float scalefactor;
	if (position.z < 0)
		scalefactor = 1; // just in case...
	else {
		//scalefactor = 1 / (sqrt(position.z) + 1);
		scalefactor = (MAX_SHADOW_HEIGHT - position.z) / MAX_SHADOW_HEIGHT; // z > MAX_SHADOW_HEIGHT will create problems...
	}
	shadow.setScale(Vector2(scalefactor, scalefactor));
	shadow.setPosition(Vector2(position.x, position.y));
}

void Creature::setPosition(const Vector3& newpos) {
	IElement3D::setPosition(newpos);
	setHitboxPosition(newpos);


	layerDepth = Map::getLayerDepth(position.y);

	// bigger th z-coord smaller the shadow
	// if z = 0 then scale = 1
	// as z -> inf scale = 0
	float scalefactor;
	if (position.z < 0)
		scalefactor = 1; // just in case...
	else {
		scalefactor = (MAX_SHADOW_HEIGHT - position.z) / MAX_SHADOW_HEIGHT; // z > MAX_SHADOW_HEIGHT will create problems...
	}
	shadow.setScale(Vector2(scalefactor, scalefactor));
	shadow.setPosition(Vector2(position.x, position.y));
}

bool Creature::isDescending() {
	return descending;
}

void Creature::stopFall() {
		falling = false;
		fallVelocity.z = 0;
		moveVelocity.z = 0;
}

void Creature::stopMovement() {
	moveVelocity.x = 0;
	moveVelocity.y = 0;
}


void Creature::moveUpdate(double deltaTime) {
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


void Creature::hitUpdate(double deltaTime) {

	if (!falling) {
		if (moveVelocity == Vector3::Zero) {
			currentFrameTime += deltaTime;
			if (currentFrameTime >= currentFrameDuration) {
				if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
					// hit sequence done
					canCancelAction = true;
					switch (facing) {
						case Facing::LEFT:
							loadAnimation(walkLeft);
							break;
						case Facing::DOWN:
							loadAnimation(walkDown);
							break;
						case Facing::RIGHT:
							loadAnimation(walkRight);
							break;
						case Facing::UP:
							loadAnimation(walkUp);
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