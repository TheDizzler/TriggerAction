#include "../pch.h"
#include "Tile.h"
#include "../Managers/GameManager.h"
#include "Creature.h"

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

TileBase::~TileBase() {
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

Tile::~Tile() {
}




void Tile::load(TileAsset* const tileAsset) {

	texture.Reset();
	texture = tileAsset->getTexture();
	width = tileAsset->getWidth();
	height = tileAsset->getHeight();

	startZposition = tileAsset->zPosition;

	origin = tileAsset->getOrigin();


	sourceRect = tileAsset->getSourceRect();

	position = Vector3(0, 0, startZposition);
	drawPosition.x = position.x;
	drawPosition.y = position.y;
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







TangibleTile::~TangibleTile() {
}

void TangibleTile::load(TileAsset* const tileAsset) {
	Tile::load(tileAsset);

	weight = 1000000;
	isFlat = tileAsset->isFlat;


	/*for (const auto& trigger : tileAsset->triggers) {
		subTriggers.push_back(make_unique<Trigger>(trigger.get()));
	}*/
	if (tileAsset->hitboxes.size() > 0) {
		for (int i = 1; i < tileAsset->hitboxes.size(); ++i)
			subHitboxes.push_back(make_unique<Hitbox>(tileAsset->hitboxes[i].get()));
		setHitbox(Hitbox(tileAsset->hitboxes[0].get()));
	}
}

void TangibleTile::takeDamage(int damage, bool showDamage) {
}


void TangibleTile::update(double deltaTime) {
	//Tile::update(deltaTime);

#ifdef  DEBUG_HITBOXES
	debugUpdate();
	//for (const auto& trigger : subTriggers)
	//	trigger->debugUpdate();
#endif //  DEBUG_HITBOXES
}


void TangibleTile::draw(SpriteBatch* batch) {
	batch->Draw(texture.Get(), drawPosition, &sourceRect, tint, rotation,
		origin, scale, SpriteEffects_None, layerDepth);

#ifdef  DEBUG_HITBOXES
	debugDraw(batch);
	//for (const auto& trigger : subTriggers)
	//	trigger->debugDraw(batch);
#endif //  DEBUG_HITBOXES
}


void TangibleTile::moveBy(const Vector3& moveVector) {

	position += moveVector;
	drawPosition.x += moveVector.x;
	drawPosition.y += moveVector.y - moveVector.z;

	moveHitboxBy(moveVector);
	/*for (auto& trigger : subTriggers)
		trigger->moveBy(moveVector);*/

	setLayerDepth(Map::getLayerDepth(position.y + maskPosition.y));
}

void TangibleTile::setPosition(const Vector3& newpos) {

	Vector3 oldpos = position;
	position = newpos;
	drawPosition.x = position.x;
	drawPosition.y = position.y - (position.z
		+ startZposition);

	setHitboxPosition(newpos);
	Vector3 moveVector = position - oldpos;
	/*for (auto& trigger : subTriggers)
		trigger->moveBy(moveVector);*/

	if (isFlat)
		setLayerDepth(Map::getLayerDepth(position.y - getHeight() + maskPosition.y));
	else
		setLayerDepth(Map::getLayerDepth(position.y + maskPosition.y));
}

const Hitbox* TangibleTile::getHitbox() const {
	return &hitbox;
}




Trigger::Trigger(int rowdata[6]) : Tangible() {
	setHitbox(rowdata);
#ifdef  DEBUG_HITBOXES
	debugSetTint(Colors::BlueViolet.v);
#endif //  DEBUG_HITBOXES
}

Trigger::Trigger(const Trigger* copytrigger) {

	setHitbox(copytrigger->getHitbox());
#ifdef  DEBUG_HITBOXES
	debugSetTint(Colors::BlueViolet.v);
#endif //  DEBUG_HITBOXES
}

Trigger::~Trigger() {
}

void Trigger::debugDraw(SpriteBatch* batch) {
	Tangible::debugDraw(batch);
}

void Trigger::moveBy(const Vector3& moveVector) {
	moveHitboxBy(moveVector);
}

void Trigger::takeDamage(int damage, bool showDamage) {
}



VerticalStepTrigger::VerticalStepTrigger(int rowdata[6]) : Trigger(rowdata) {
}

VerticalStepTrigger::VerticalStepTrigger(const VerticalStepTrigger* copyTrigger)
	: Trigger(copyTrigger) {
}

VerticalStepTrigger::~VerticalStepTrigger() {
}

bool VerticalStepTrigger::activateTrigger(Creature* creature) {

	if (hitbox.collision2d(creature->getHitbox())) {


		// min height = hitbox.position.z
		// max height = hitbox.size.z
		Vector3 newpos = creature->getPosition();
		float relativeY = (hitbox.position.y + hitbox.size.y + 1)
			- (newpos.y - creature->getHitbox()->size.y);
		float percent = relativeY / hitbox.size.y;
		float newZ = (hitbox.size.z - hitbox.position.z) * percent + hitbox.position.z;

		float dif = newpos.z - (hitbox.position.z + newZ);

		if (dif < LANDING_TOLERANCE * 2
			&& dif > -LANDING_TOLERANCE * 2
			&& creature->isDescending()) {

			newpos.z = newZ;
			creature->setPosition(newpos);
			creature->stopFall();
			return true;
		}




	}
	return false;
}

HorizontalStepTrigger::HorizontalStepTrigger(int rowdata[6]) : Trigger(rowdata) {
}

HorizontalStepTrigger::HorizontalStepTrigger(const HorizontalStepTrigger* copyTrigger)
	: Trigger(copyTrigger) {
}

HorizontalStepTrigger::~HorizontalStepTrigger() {
}

bool HorizontalStepTrigger::activateTrigger(Creature* creature) {
	if (hitbox.collision2d(creature->getHitbox())) {


		// min height = hitbox.position.z
		// max height = hitbox.size.z + hitbox.position.z
		Vector3 newpos = creature->getPosition();

		float relativeX;
		float percent;
		float newZ;

		if (rightUp) {
			relativeX = (newpos.x + creature->getHitbox()->size.x / 2 + 1.5) - (hitbox.position.x);
			/*if (relativeX < 0)
				return false;*/

		} else {
			relativeX = (hitbox.position.x + hitbox.size.x) - (newpos.x - creature->getHitbox()->size.x / 2);

		}

		percent = ceil(relativeX) / hitbox.size.x;
		newZ = (hitbox.size.z) * percent + hitbox.position.z;

		float dif = newpos.z - (newZ);

		if (dif < LANDING_TOLERANCE * 2
			&& dif > -LANDING_TOLERANCE * 2
			&& creature->isDescending()) {

			newpos.z = newZ;
			creature->setPosition(newpos);
			creature->stopFall();
			return true;
		}




	}
	return false;
}
