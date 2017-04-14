#include "../pch.h"
#include "Tile.h"


#include "../Managers/MapManager.h"
void TileBase::moveBy(const Vector3 & moveVector) {

	position += moveVector;
	drawPosition.x += moveVector.x;
	drawPosition.y += moveVector.y - moveVector.z;

	if (layerDepth >= FURTHEST_DEPTH && layerDepth <= NEAREST_DEPTH)
		setLayerDepth(Map::getLayerDepth(position.y + maskPosition.y));
}

void TileBase::setPosition(const Vector3& newpos) {

	position = newpos;
	drawPosition.x = position.x;
	drawPosition.y = (position.y - position.z);

	if (layerDepth >= FURTHEST_DEPTH && layerDepth <= NEAREST_DEPTH)
		setLayerDepth(Map::getLayerDepth(position.y + maskPosition.y));
}

void TileBase::update(double deltaTime) {
}




Tile::Tile() {

	rotation = 0.0f;
	scale = Vector2(1, 1);
	tint = DirectX::Colors::White;
	layerDepth = 0.0f;

	width = 0;
	height = 0;
}




void Tile::load(TileAsset* const tileAsset) {

	texture.Reset();
	texture = tileAsset->getTexture();
	width = tileAsset->getWidth();
	height = tileAsset->getHeight();

	origin = tileAsset->getOrigin();


	sourceRect = tileAsset->getSourceRect();

	position = Vector3(0, 0, 0);
	drawPosition.x = position.x;
	drawPosition.y = position.y/* - height*/;
	maskPosition = tileAsset->mask;
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
	drawPosition.y += moveVector.y - moveVector.z;
	
	hitbox->position += moveVector;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveVector;

	setLayerDepth(Map::getLayerDepth(position.y + maskPosition.y));
}

void TangibleTile::setPosition(const Vector3& newpos) {

	Vector3 moveBy = newpos - position;
	position = newpos;
	drawPosition.x = position.x;
	drawPosition.y = position.y - position.z;

	hitbox->position += moveBy;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveBy;

	setLayerDepth(Map::getLayerDepth(position.y + maskPosition.y));
}

const Hitbox* TangibleTile::getHitbox() const {
	return hitbox.get();
}

