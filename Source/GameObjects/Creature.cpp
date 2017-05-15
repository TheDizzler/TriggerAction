#include "../pch.h"
#include "Creature.h"
#include "../Screens/LevelScreen.h"


Creature::Creature() {
	layerDepth = 0.1f;
}

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

const int Creature::getHeight() const {
	return currentAnimation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.top;
}

const int Creature::getWidth() const {
	return currentAnimation->animationFrames[currentFrameIndex]->sourceRect.right
		- currentAnimation->animationFrames[currentFrameIndex]->sourceRect.left;
}

const Hitbox* Creature::getHitbox() const {
	return &hitbox;
}

void Creature::moveBy(const Vector3& moveVector) {
	IElement3D::moveBy(moveVector);
	moveHitboxBy(moveVector);

	layerDepth = Map::getLayerDepth(position.y);

}

void Creature::setPosition(const Vector3& newpos) {
	IElement3D::setPosition(newpos);
	setHitboxPosition(newpos);

	layerDepth = Map::getLayerDepth(position.y);
	
}