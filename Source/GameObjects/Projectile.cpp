#include "../pch.h"
#include "Projectile.h"
#include "../Screens/LevelScreen.h"
#include "../Engine/GameEngine.h"

Projectile::Projectile(Vector3 wPos[4]) {
	memmove(weaponPositions, wPos, sizeof(weaponPositions));
}

Projectile::~Projectile() {
}

void Projectile::loadBullet(shared_ptr<Animation> bullet, GraphicsAsset* shd) {
	projectileLeft = bullet;
	shadow = shd;
	shadow->getOrigin();
	shadowOrigin = Vector2((float) shadow->getWidth() / 2, (float) shadow->getHeight() / 2);
	//shadowOrigin = Vector2::Zero;
}

void Projectile::loadHitEffect(shared_ptr<Animation> hitFx) {
	hitEffect = hitFx;
}

void Projectile::update(double deltaTime) {
	if (!isActive)
		return;

	Vector2 screenpos = camera->worldToScreen(Vector2(position.x, position.y));
	if (screenpos.x < 0 || screenpos.x > Globals::WINDOW_WIDTH
		|| screenpos.y < 0 || screenpos.y > Globals::WINDOW_HEIGHT) {

		isActive = false;
	}

	// ray-casting hit detection
	Vector2 p = position; // raystart pos
	for (const Hitbox* hitbox : hitboxesAll) {

	/*	Vector2 a(hitbox->position.x, hitbox->position.y + hitbox->size.y);
		Vector2 b(hitbox->position.x + hitbox->size.x, hitbox->position.y);*/


	}

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= projectileLeft->animationFrames.size())
			currentFrameIndex = 0;
		currentFrameTime = 0;
		Frame* frame = projectileLeft->animationFrames[currentFrameIndex].get();
		currentFrameDuration = frame->frameTime;
	}

	moveBy(Vector3(-cos(rotation)*projectileSpeed*deltaTime,
		sin(-rotation)* projectileSpeed*deltaTime, 0));
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
	IElement3D::setPosition(tempos);
	shadowPosition = pos;
	shadowRotation = rotation;

	switch (direction) {
		case Facing::LEFT:
		case Facing::RIGHT:
			shadowPosition.x = position.x;
			break;
		case Facing::UP:
		case Facing::DOWN:
			shadowPosition.y = position.y;
			break;
	}
	layerDepth = Map::getLayerDepth(position.y);
}

void Projectile::store() {
	isActive = false;
}

void Projectile::moveBy(const Vector3& moveVector) {
	IElement3D::moveBy(moveVector);
	shadowPosition += moveVector;
}


const int Projectile::getWidth() const {
	return projectileLeft->animationFrames[currentFrameIndex]->sourceRect.right
		- projectileLeft->animationFrames[currentFrameIndex]->sourceRect.left;
}

const int Projectile::getHeight() const {
	return projectileLeft->animationFrames[currentFrameIndex]->sourceRect.bottom
		- projectileLeft->animationFrames[currentFrameIndex]->sourceRect.top;
}
