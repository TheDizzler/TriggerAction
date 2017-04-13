#include "../pch.h"
#include "Tile.h"


Tile::Tile() {

	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White;
	layerDepth = 0.0f;

	width = 0;
	height = 0;
}



#include "../Managers/MapManager.h"
void Tile::load(TileAsset* const tileAsset) {

	texture.Reset();
	texture = tileAsset->getTexture();
	width = tileAsset->getWidth();
	height = tileAsset->getHeight();

	origin = tileAsset->getOrigin();


	sourceRect = tileAsset->getSourceRect();


}


void Tile::draw(SpriteBatch* batch) {
	batch->Draw(texture.Get(), drawPosition, &sourceRect, tint, rotation,
		origin, scale, SpriteEffects_None, layerDepth);
}


const int Tile::getWidth() const {
	return width * scale.x;
}

const int Tile::getHeight() const {
	return height * scale.y;
}





void TangibleTile::load(TileAsset * const tileAsset) {
	Tile::load(tileAsset);

	if (tileAsset->hitboxes.size() > 0) {
		hitbox = make_unique<Hitbox>(tileAsset->hitboxes[0].get());
		for (const auto& hitbox : tileAsset->hitboxes) {
			unique_ptr<Hitbox> hb = make_unique<Hitbox>(hitbox.get());
			subHitboxes.push_back(move(hb));
		}
	}
}

bool TangibleTile::checkCollisionWith(const Hitbox* other) const {
	
	if (hitbox->collision2d(other)) { // first check to see if hitbox overlap on x-y plane
		if (hitbox->collisionZ(other)) // then check if collide on z-axis as well
			return true;
		for (const auto& subHB : subHitboxes) // then check inner hitboxes for collisions
			if (subHB->collision(other))
				return true;
	}
	return false;
}

void TangibleTile::moveBy(const Vector3& moveVector) {

	position += moveVector;
	drawPosition.x += moveVector.x;
	drawPosition.y += moveVector.y + moveVector.z;
	hitbox->position += moveVector;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveVector;
}

void TangibleTile::setPosition(const Vector3& newpos) {

	Vector3 moveBy = position - newpos;
	position = newpos;
	drawPosition.x = position.x;
	drawPosition.y = position.y + position.z;


	hitbox->position += moveBy;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveBy;
}

const Hitbox* TangibleTile::getHitbox() const {
	return hitbox.get();
}





