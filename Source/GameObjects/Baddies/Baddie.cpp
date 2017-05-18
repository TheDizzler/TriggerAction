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

	/*currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= currentAnimation->animationFrames.size())
			currentFrameIndex = 0;
		currentFrameTime = 0;
		currentFrameDuration
			= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
		currentFrameRect
			= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
		currentFrameOrigin
			= currentAnimation->animationFrames[currentFrameIndex]->origin;
	}*/

	switch (action) {
		case CreatureAction::WAITING_ACTION:
			currentFrameTime += deltaTime;
			if (currentFrameTime >= currentFrameDuration) {
				if (++currentFrameIndex >= currentAnimation->animationFrames.size())
					currentFrameIndex = 0;
				currentFrameTime = 0;
				currentFrameDuration
					= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
				currentFrameRect
					= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
				currentFrameOrigin
					= currentAnimation->animationFrames[currentFrameIndex]->origin;
			}
			break;
		case CreatureAction::MOVING_ACTION:
		/*	if (joystick->bButtonStates[ControlButtons::Y]) {
				startMainAttack();
			} else {
				movement(deltaTime);

			}
			if (!stillAttacking)
				break;
			else*/
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
}
