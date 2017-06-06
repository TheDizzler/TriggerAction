#include "../pch.h"
#include "Creature.h"
#include "../Screens/LevelScreen.h"


Creature::Creature() {
	layerDepth = 0.1f;
}

Creature::~Creature() {
}





void Creature::loadAnimation(shared_ptr<Animation> animation) {
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
	//currentFrameTexture = currentAnimation->texture.Get(); // shouldn't change
}

bool Creature::checkCollisionWith(const Hitbox* hitbox) const {
	return false;
}

const int Creature::getHeight() const {
	return currentFrameRect.bottom - currentFrameRect.top;
}

const int Creature::getWidth() const {
	return currentFrameRect.right - currentFrameRect.left;
}

const Hitbox* Creature::getHitbox() const {
	return &hitbox;
}

void Creature::moveBy(const Vector3& moveVector) {
	IElement3D::moveBy(moveVector);
	moveHitboxBy(moveVector);
	shadow.moveBy(Vector2(moveVector.x, moveVector.y));

	layerDepth = Map::getLayerDepth(position.y);

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
	else
		//scalefactor = 1 / (sqrt(position.z) + 1);
		scalefactor = (100 - position.z) / 100; // z > 100 will create problems...
	shadow.setScale(Vector2(scalefactor, scalefactor));
	shadow.setPosition(Vector2(position.x, position.y));
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

	if (knockBackVelocity != Vector3::Zero) {
		moveBy(knockBackVelocity);
		knockBackVelocity += GRAVITY * deltaTime;
		if (position.z <= 0) {
			knockBackVelocity = Vector3::Zero;
		}
	} else {
		currentFrameTime += deltaTime;
		if (currentFrameTime >= currentFrameDuration) {
			if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
				// hit sequence done
				canCancelAction = true;
				loadAnimation(walkLeft);
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
