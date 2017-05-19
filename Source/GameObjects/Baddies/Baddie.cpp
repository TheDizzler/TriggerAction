#include "../../pch.h"
#include "Baddie.h"
#include "../../Managers/MapManager.h"

BaddieData::BaddieData() {
}

BaddieData::~BaddieData() {
}

void BaddieData::loadData(xml_node baddieDataNode, shared_ptr<AssetSet> assetSet) {

	xml_node hitboxNode = baddieDataNode.child("hitbox");

	int rowdata[5] = {
		hitboxNode.attribute("x").as_int(), hitboxNode.attribute("y").as_int(),
		hitboxNode.attribute("width").as_int(), hitboxNode.attribute("height").as_int(),
		hitboxNode.attribute("z").as_int()
	};

	hitbox = make_unique<Hitbox>(rowdata);
	assets = assetSet;
}


#include "../../Screens/LevelScreen.h"
Baddie::Baddie(BaddieData* data) {

	setHitbox(data->hitbox.get());
	hitboxesAll.push_back(this);

	threatRange = Vector3(25, 25, 20);
	jumpSpeed = 85;

	assetSet = data->assets;
	walkDown = assetSet->getAnimation("walk down");
	walkLeft = assetSet->getAnimation("walk left");
	walkUp = assetSet->getAnimation("walk up");
	walkRight = assetSet->getAnimation("walk right");

	attackDown = assetSet->getAnimation("attack down");
	attackLeft = assetSet->getAnimation("attack left");
	attackUp = assetSet->getAnimation("attack up");
	attackRight = assetSet->getAnimation("attack right");

	provoke = assetSet->getAnimation("provoke");
	surprise = assetSet->getAnimation("surprise");
	hit = assetSet->getAnimation("hit");

	loadAnimation(walkLeft);
	currentFrameTexture = currentAnimation->texture.Get();
}

Baddie::~Baddie() {
}


void Baddie::update(double deltaTime) {

	switch (action) {
		case CreatureAction::WAITING_ACTION:
			currentFrameTime += deltaTime;
			if (currentFrameTime >= currentFrameDuration) {
				if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
					currentFrameIndex = 0;
				}
				currentFrameTime = 0;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;
			}
			for (const auto& pc : pcs) {
				Vector3 distance = pc->getHitbox()->position - hitbox.position;

				if (abs(distance.x) < threatRange.x && abs(distance.y) < threatRange.y) {
					// le petit attaque
					distance.Normalize();
					startMainAttack(distance);
				}

			}
			break;
		case CreatureAction::MOVING_ACTION:
			break;
		case CreatureAction::ATTACKING_ACTION:
			attackUpdate(deltaTime);

			break;

		case CreatureAction::HIT_ACTION:
			hitUpdate(deltaTime);
			break;

	}

	debugUpdate();
}

void Baddie::draw(SpriteBatch* batch) {

	batch->Draw(currentFrameTexture, drawPosition,
		&currentFrameRect, tint, rotation,
		currentFrameOrigin, scale,
		SpriteEffects_None, layerDepth);


	debugDraw(batch);
}

void Baddie::attackUpdate(double deltaTime) {
	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
			// attack sequence completed
			canCancelAction = true;
			loadAnimation(walkLeft);
			action = CreatureAction::WAITING_ACTION;
			return;
		}
		currentFrameTime = 0;
		currentFrameDuration
			= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
		currentFrameRect
			= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
		currentFrameOrigin
			= currentAnimation->animationFrames[currentFrameIndex]->origin;
	}


	switch (currentFrameIndex) {
		case 0:
			break;
		case 1:
			// jump forward
			moveBy(jumpVelocity * deltaTime);
			/*for (auto& pc : pcs) {
				if (hitbox.collision

			}*/
			break;


	}
}

void Baddie::startMainAttack(Vector3 direction) {

	action = CreatureAction::ATTACKING_ACTION;
	canCancelAction = false;

	// get direction to attack in
	if (abs(direction.x) > abs(direction.y)) {
		// attack more horizontal than vertical
		if (direction.x < 0) {
			// attack going left
			facing = Facing::LEFT;
			loadAnimation(attackLeft);
		} else {
			// attack going right
			facing = Facing::RIGHT;
			loadAnimation(attackRight);
		}
	} else {
		// attack more vertical than horizontal
		if (direction.y < 0) {
			// attack going up
			facing = Facing::UP;
			loadAnimation(attackUp);
		} else {
			// attack going down
			facing = Facing::DOWN;
			loadAnimation(attackDown);
		}
	}

	jumpVelocity = direction * jumpSpeed;


	currentFrameTime = 0;
	currentFrameDuration
		= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
	currentFrameRect
		= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
	currentFrameOrigin
		= currentAnimation->animationFrames[currentFrameIndex]->origin;
}
