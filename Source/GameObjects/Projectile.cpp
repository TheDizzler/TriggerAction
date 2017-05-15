#include "../pch.h"
#include "Projectile.h"
#include "../Screens/LevelScreen.h"

Projectile::Projectile(Vector3 wPos[4]) {
	memmove(weaponPositions, wPos, sizeof(weaponPositions));
}

Projectile::~Projectile() {
}

void Projectile::loadBullet(shared_ptr<Animation> bullet) {
	projectileLeft = bullet;
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
}

void Projectile::fire(Facing direction, const Vector3& pos) {
	isActive = true;
	currentFrameTime = 0;

	rotation = direction * -XM_PIDIV2;
	
	Vector3 tempos = pos + weaponPositions[direction];
	//tempos.x += weaponPositions[direction].x;
	//tempos.y += weaponPositions[direction].z;
	setPosition(tempos);

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
