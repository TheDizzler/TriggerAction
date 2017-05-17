#include "../pch.h"
#include "Projectile.h"
#include "../Screens/LevelScreen.h"
#include "../Engine/GameEngine.h"

Projectile::Projectile(Creature* ownr, Vector3 wPos[4]) {
	memmove(weaponPositions, wPos, sizeof(weaponPositions));

	owner = ownr;
	ownerBox = owner->getHitbox();
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
	for (Tangible* liveObject : hitboxesAll) {

		if (liveObject == owner)
			continue;
		// check if ray intersects with hitbox
		if (ray.collision2d(liveObject->getHitbox())) {
			//int i = 0;
			liveObject->debugSetTint(Colors::Cyan.v);
		} else
			liveObject->debugSetTint(Colors::Black.v);


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

void Projectile::fire(Facing dir, const Vector3& pos) {
	isActive = true;
	currentFrameTime = 0;

	direction = dir;
	rotation = direction * -XM_PIDIV2;

	Vector3 tempos = pos + weaponPositions[direction];
	IElement3D::setPosition(tempos);
	shadowPosition = pos;
	shadowRotation = rotation;
	ray.position = position;
	// multiplier makes the hit detection a little more generous
	//	if this is changed then the ray.position adjustments belo will
	//	proportionally need to be changed
	ray.size = Vector3(getHeight() * 2, getHeight() * 2, getHeight() * 2);
	switch (direction) {
		case Facing::LEFT:
			shadowPosition.x = position.x;
			ray.position.x = camera->screenToWorld(Vector2::Zero).x;
			ray.size.x = position.x - ray.position.x;
			break;
		case Facing::RIGHT:
			ray.size.x = Globals::WINDOW_WIDTH;
			ray.position.y -= getHeight();
			break;
		case Facing::UP:
			shadowPosition.y = position.y;
			ray.position.y = camera->screenToWorld(Vector2::Zero).y;
			ray.size.y = position.y - ray.position.y;
			break;
		case Facing::DOWN:
			ray.size.y = Globals::WINDOW_HEIGHT;
			ray.position.x -= getHeight();
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
