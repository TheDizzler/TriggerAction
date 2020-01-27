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


void Projectile::loadHitEffect(Animation* hitFx) {
	hitEffect = hitFx;
}


void Projectile::update(double deltaTime) {
	if (!isActive)
		return;

	if (isExploding) {

		currentFrameTime += deltaTime;
		if (currentFrameTime >= currentFrameDuration) {
			if (++currentFrameIndex >= hitEffect->animationFrames.size()) {
				isActive = false;
				isExploding = false;
				return;
			}
			currentFrameTime = 0;
			currentFrameDuration
				= hitEffect->animationFrames[currentFrameIndex].get()->frameTime;
		}
		return;
	}

	Vector2 screenpos = camera.worldToScreen(Vector2(position.x, position.y));
	if (screenpos.x < 0 || screenpos.x > Globals::WINDOW_WIDTH
		|| screenpos.y < 0 || screenpos.y > Globals::WINDOW_HEIGHT) {

		isActive = false;
		return;
	}

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= projectileLeft->animationFrames.size())
			currentFrameIndex = 0;
		currentFrameTime = 0;
		currentFrameDuration
			= projectileLeft->animationFrames[currentFrameIndex]->frameTime;
		currentFrameRect
			= projectileLeft->animationFrames[currentFrameIndex]->sourceRect;
		currentFrameOrigin
			= projectileLeft->animationFrames[currentFrameIndex]->origin;
	}

	moveBy(Vector3(-cos(rotation)*projectileSpeed*deltaTime,
		sin(-rotation)* projectileSpeed*deltaTime, 0));

	// ray-casting hit detection
	for (Tangible* liveObject : tangiblesAll) {

		if (liveObject == owner)
			continue;

#ifdef  DEBUG_HITBOXES
		liveObject->debugSetTint(Colors::Black.v);
#endif //  DEBUG_HITBOXES

		if (ray.collision2d(liveObject->getHitbox())) {
			if (ray.collisionZ(liveObject->getHitbox())) {
#ifdef  DEBUG_HITBOXES
				liveObject->debugSetTint(Colors::Cyan.v);
#endif //  DEBUG_HITBOXES
				if (fineHitDetection(liveObject->getHitbox())) {
					hit(liveObject);
#ifdef  DEBUG_HITBOXES
					liveObject->debugSetTint(Colors::Crimson.v);
#endif //  DEBUG_HITBOXES
					break;
				}
			} else {
				for (const auto& subhb : liveObject->subHitboxes) {
#ifdef  DEBUG_HITBOXES
					liveObject->debugSetTint(Colors::Cyan.v);
#endif //  DEBUG_HITBOXES
					if (ray.collision(subhb.get())) {
						if (fineHitDetection(subhb.get())) {
							hit(liveObject);
#ifdef  DEBUG_HITBOXES
							liveObject->debugSetTint(Colors::Crimson.v);
#endif //  DEBUG_HITBOXES
							break;
						}
					}

				}

			}
		}
	}
}

void Projectile::draw(SpriteBatch* batch) {
	if (!isActive)
		return;

	if (isExploding) {

		batch->Draw(hitEffect->texture.Get(), drawPosition,
			&hitEffect->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
			hitEffect->animationFrames[currentFrameIndex]->origin, scale,
			SpriteEffects_None, layerDepth);
		return;
	}

	batch->Draw(currentFrameTexture, drawPosition,
		&currentFrameRect, tint, rotation,
		currentFrameOrigin, scale,
		SpriteEffects_None, layerDepth);

	batch->Draw(shadow->getTexture().Get(), shadowPosition,
		&shadow->getSourceRect(), tint, rotation,/* shadowOrigin*/currentFrameOrigin,
		scale, SpriteEffects_None, .1);
}


void Projectile::fire(Facing dir, const Vector3& pos) {
	isActive = true;
	currentFrameTime = 0;
	currentFrameIndex = 0;
	currentFrameDuration
		= projectileLeft->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect
		= projectileLeft->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin
		= projectileLeft->animationFrames[currentFrameIndex]->origin;

	direction = dir;
	rotation = direction * -XM_PIDIV2;

	Vector3 tempos = pos + weaponPositions[direction];
	IElement3D::setPosition(tempos);
	shadowPosition = pos + weaponPositions[direction];
	ray.position = position;
	// multiplier makes the hit detection a little more generous
	//	if this is changed then the ray.position adjustments below will
	//	proportionally need to be changed
	ray.size = Vector3(getHeight(), getHeight() * 2, getHeight() * 2);
	switch (direction) {
		case Facing::LEFT:
			ray.position.x = camera.screenToWorld(Vector2::Zero).x;
			ray.position.y -= getHeight() * 2;
			ray.size.x = position.x - ray.position.x;
			break;
		case Facing::RIGHT:
			ray.position.y -= getHeight() * 2;
			ray.size.x = Globals::WINDOW_WIDTH;
			break;
		case Facing::UP:
			ray.position.y = camera.screenToWorld(Vector2::Zero).y;
			ray.size.y = position.y - ray.position.y;
			break;
		case Facing::DOWN:
			ray.position.x -= getHeight() / 2;
			ray.size.y = Globals::WINDOW_HEIGHT;
			break;
	}

	moveInOneFrame = Vector2(-cos(rotation) * distanceDeltaPerFrame,
		sin(-rotation) * distanceDeltaPerFrame);
	layerDepth = Map::getLayerDepth(position.y);
}

bool Projectile::fineHitDetection(const Hitbox* hb) {

	int xDist, yDist;
	switch (direction) {
		case Facing::LEFT:
			xDist = hb->position.x - position.x;
			if (xDist > moveInOneFrame.x - hb->size.x / 2) {

				return true;
			}

			break;
		case Facing::RIGHT:
			xDist = position.x - hb->position.x;
			if (xDist > moveInOneFrame.x - hb->size.x) {
				return true;
			}

			break;

		case Facing::DOWN:
			yDist = position.y - hb->position.y;
			if (yDist > moveInOneFrame.y - hb->size.y / 2) {
				return true;
			}

			break;
		case Facing::UP:
			yDist = hb->position.y - position.y;
			if (yDist > moveInOneFrame.y - hb->size.z / 2) {
				return true;
			}

			break;
	}

	return false;
}


void Projectile::hit(Tangible* liveObject) {

	liveObject->takeDamage(damage);
	layerDepth = Map::getLayerDepth(liveObject->getHitbox()->position.y
		+ liveObject->getHitbox()->size.y + 1);
	isExploding = true;
	currentFrameTime = 0;
	currentFrameIndex = 0;
	currentFrameDuration = hitEffect->animationFrames[currentFrameIndex]->frameTime;
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



BronzeBow::BronzeBow(Creature* ownr, AssetSet* weaponSet, Vector3 weaponPositions[4])
	: Projectile(ownr, weaponPositions) {

	loadBullet(weaponSet->getAnimation("Bronze Bow Left"),
		weaponSet->getAsset("Bullet Shadow"));
	loadHitEffect(weaponSet->getAnimation("Bronze Bow HitEffect"));
}

BronzeBow::~BronzeBow() {
}

void BronzeBow::loadBullet(Animation* bullet, GraphicsAsset* shd) {

	projectileLeft = bullet;
	currentFrameTexture = projectileLeft->texture.Get();
	shadow = shd;

	damage = 5;

	projectileSpeed = 250;
	distanceDeltaPerFrame = projectileSpeed / 60;
}



AirGun::AirGun(Creature* ownr, AssetSet* weaponSet, Vector3 weaponPositions[4])
	: Projectile(ownr, weaponPositions) {

	loadBullet(weaponSet->getAnimation("AirGun Bullet Left"),
		weaponSet->getAsset("Bullet Shadow"));
	loadHitEffect(weaponSet->getAnimation("AirGun HitEffect"));
}

AirGun::~AirGun() {
}

void AirGun::loadBullet(Animation* bullet, GraphicsAsset* shd) {

	projectileLeft = bullet;
	currentFrameTexture = projectileLeft->texture.Get();
	shadow = shd;

	damage = 5;

	projectileSpeed = 250;
	distanceDeltaPerFrame = projectileSpeed / 60;
}
