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
	hitboxesAll.push_back(getHitbox());

	shared_ptr<AssetSet> assets = data->assets;
	walkDown = assets->getAnimation("walk down");
	walkLeft = assets->getAnimation("walk left");
	walkUp = assets->getAnimation("walk up");
	walkRight = assets->getAnimation("walk right");

	attackDown = assets->getAnimation("attack down");
	attackLeft = assets->getAnimation("attack left");
	attackUp = assets->getAnimation("attack up");
	attackRight = assets->getAnimation("attack right");

	provoke = assets->getAnimation("provoke");
	surprise = assets->getAnimation("surprise");
	hit = assets->getAnimation("hit");

	currentAnimation = walkLeft;
	currentFrameIndex = 0;
	currentFrameTime = 0;
	currentFrameDuration = currentAnimation->animationFrames[currentFrameIndex]->frameTime;
}

Baddie::~Baddie() {
}


void Baddie::update(double deltaTime) {

	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= currentAnimation->animationFrames.size())
			currentFrameIndex = 0;
		currentFrameTime = 0;
		Frame* frame = currentAnimation->animationFrames[currentFrameIndex].get();
		currentFrameDuration = frame->frameTime;
	}
}

void Baddie::draw(SpriteBatch* batch) {

	batch->Draw(currentAnimation->texture.Get(), position,
		&currentAnimation->animationFrames[currentFrameIndex]->sourceRect, tint, rotation,
		currentAnimation->animationFrames[currentFrameIndex]->origin, scale,
		SpriteEffects_None, layerDepth);


	debugDraw(batch);
}