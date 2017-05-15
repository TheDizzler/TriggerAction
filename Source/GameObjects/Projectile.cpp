#include "../pch.h"
#include "Projectile.h"


Projectile::~Projectile() {
}

void Projectile::loadBullet(shared_ptr<Animation> bullet) {
	projectileLeft = bullet;
}

void Projectile::loadHitEffect(shared_ptr<Animation> hitFx) {
	hitEffect = hitFx;
}

void Projectile::update(double deltaTime) {
}

void Projectile::draw(SpriteBatch* batch) {
}

const int Projectile::getWidth() const {
	return 0;
}

const int Projectile::getHeight() const {
	return 0;
}
