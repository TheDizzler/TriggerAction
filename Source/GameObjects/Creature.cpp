#include "../pch.h"
#include "Creature.h"
#include "../Screens/LevelScreen.h"


Creature::~Creature() {
}


void Creature::loadAnimation(const pugi::char_t* name) {

	currentAnimation = assetSet->getAnimation(name);
	currentFrameIndex = 0;
	currentFrameTime = 0;
	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;

}

bool Creature::checkCollisionWith(const Hitbox* hitbox) const {
	return false;
}

int Creature::getHeight() const {
	return currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
}

int Creature::getWidth() const {
	return currentAnimation->animationFrames[currentFrameIndex]->sourceRect.right
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.left;
}

const Hitbox* Creature::getHitbox() const {
	return &hitbox;
}

void Creature::moveBy(const Vector3& moveVector) {

	position += moveVector;
	moveHitboxBy(moveVector);

	layerDepth = Map::getLayerDepth(position.y);
	debugUpdate(Vector2(moveVector.x, moveVector.y));

}

void Creature::setPosition(const Vector3& newpos) {

	position = newpos;

	setHitboxPosition(newpos);

	layerDepth = Map::getLayerDepth(position.y);
	debugUpdate(Vector2(newpos.x, newpos.y));
}