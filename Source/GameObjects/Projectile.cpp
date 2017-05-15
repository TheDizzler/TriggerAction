#include "../pch.h"
#include "Projectile.h"
#include "../Screens/LevelScreen.h"

Projectile::Projectile(Vector3 wPos[4]) {
	memmove(weaponPositions, wPos, sizeof(weaponPositions));
}

Projectile::~Projectile() {
}

void Projectile::loadBullet(shared_ptr<Animation> bullet, GraphicsAsset* shd) {
	projectileLeft = bullet;
	shadow = shd;
	shadow->getOrigin();
	//shadowOrigin = Vector2((float) shadow->getWidth()/2, (float) shadow->getHeight()/2);
	shadowOrigin = Vector2::Zero;
}

void Projectile::loadHitEffect(shared_ptr<Animation> hitFx) {
	hitEffect = hitFx;
}

void Projectile::update(double deltaTime) {
	if (!isActive)
		return;


	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= projectileLeft->animationFrames.size())
			currentFrameIndex = 0;
		currentFrameTime = 0;
		Frame* frame = projectileLeft->animationFrames[currentFrameIndex].get();
		currentFrameDuration = frame->frameTime;
	}
}

void Projectile::draw(SpriteBatch* batch) {
	if (!isActive)
		return;

	batch->Draw(projectileLeft->texture.Get(), drawPosition,
		&projectileLeft->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		projectileLeft->animationFrames[currentFrameIndex]->origin, scale,
		SpriteEffects_None, layerDepth);

	batch->Draw(shadow->getTexture().Get(), shadowPosition,
		&shadow->getSourceRect(), tint, shadowRotation, shadowOrigin,
		scale, SpriteEffects_None, .1);
}

void Projectile::fire(Facing direction, const Vector3& pos) {
	isActive = true;
	currentFrameTime = 0;

	rotation = direction * -XM_PIDIV2;

	Vector3 tempos = pos + weaponPositions[direction];
	setPosition(tempos);
	shadowPosition = pos;
	shadowPosition.x = position.x;
	switch (direction) {
		case Facing::LEFT:
		case Facing::RIGHT:
			shadowRotation = 0;
			break;
		case Facing::UP:
			shadowRotation = -rotation;
			break;
		case Facing::DOWN:
			shadowRotation = rotation;
			break;
	}
	layerDepth = Map::getLayerDepth(position.y);
}

void Projectile::store() {
	isActive = false;
}

const int Projectile::getWidth() const {
	return projectileLeft->animationFrames[currentFrameIndex]->sourceRect.right
		- projectileLeft->animationFrames[currentFrameIndex]->sourceRect.left;
}

const int Projectile::getHeight() const {
	return projectileLeft->animationFrames[currentFrameIndex]->sourceRect.bottom
		- projectileLeft->animationFrames[currentFrameIndex]->sourceRect.top;
}
