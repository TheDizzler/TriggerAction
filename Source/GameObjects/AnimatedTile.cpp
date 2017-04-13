#include "../pch.h"
#include "AnimatedTile.h"


AnimatedTile::AnimatedTile() {
	
	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White;
	layerDepth = 0.0f;

}



AnimatedTile::~AnimatedTile() {
}

void AnimatedTile::load(shared_ptr<Animation> anim) {

	animation = anim;

	if (animation->animationFrames.size() > 0)
		currentFrameIndex = 0;


}


void AnimatedTile::update(double deltaTime) {

	currentFrameTime += deltaTime;
	if (currentFrameTime >= animation->timePerFrame) {
		if (++currentFrameIndex >= animation->animationFrames.size())
			if (repeats)
				currentFrameIndex = 0;
			else
				isAlive = false;
		currentFrameTime = 0;
	}
}

void AnimatedTile::draw(SpriteBatch* batch) {
	batch->Draw(animation->texture.Get(), drawPosition,
		&animation->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		origin, scale, SpriteEffects_None, layerDepth);
}



const RECT AnimatedTile::getRect() const {
	return animation->animationFrames[currentFrameIndex]->sourceRect;
}

const int AnimatedTile::getWidth() const {
	return animation->animationFrames[currentFrameIndex]->sourceRect.right
		- animation->animationFrames[currentFrameIndex]->sourceRect.left;
}

const int AnimatedTile::getHeight() const {
	return animation->animationFrames[currentFrameIndex]->sourceRect.bottom
		- animation->animationFrames[currentFrameIndex]->sourceRect.top;
}



void AnimatedTile::reset() {
	isAlive = true;
	currentFrameIndex = 0;
}
