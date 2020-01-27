#include "../../pch.h"
#include "CharacterData.h"
#include "../../Engine/GameEngine.h"
#include "../../Managers/GFXAssetManager.h"


CharacterData::CharacterData(string nm) {

	name = nm;
	assets = gfxAssets->getAssetSet(name.c_str());
}

CharacterData::~CharacterData() {
}


void CharacterData::loadData(xml_node characterDataNode,
	AssetSet* assetSet, AssetSet* weaponSet) {

	xml_node hitboxNode = characterDataNode.child("hitbox");

	int rowdata[6] = {
		hitboxNode.attribute("x").as_int(), hitboxNode.attribute("y").as_int(),
		hitboxNode.attribute("z").as_int(),
		hitboxNode.attribute("width").as_int(), hitboxNode.attribute("height").as_int(),
		hitboxNode.attribute("zHeight").as_int()};

	hitbox = make_unique<Hitbox>(rowdata);

	xml_node weapPosNode = characterDataNode.child("weaponPosition");
	xml_node down = weapPosNode.child("down");
	xml_node left = weapPosNode.child("left");
	xml_node up = weapPosNode.child("up");
	xml_node right = weapPosNode.child("right");
	weaponPositions[Facing::DOWN] = Vector3(down.attribute("x").as_int(),
		down.attribute("y").as_int(), down.attribute("z").as_int());
	weaponPositions[Facing::LEFT] = Vector3(left.attribute("x").as_int(),
		left.attribute("y").as_int(), left.attribute("z").as_int());
	weaponPositions[Facing::UP] = Vector3(up.attribute("x").as_int(),
		up.attribute("y").as_int(), up.attribute("z").as_int());
	weaponPositions[Facing::RIGHT] = Vector3(right.attribute("x").as_int(),
		right.attribute("y").as_int(), right.attribute("z").as_int());


	xml_node attackBoxSize = characterDataNode.child("attackBoxSizes");
	down = attackBoxSize.child("down");
	left = attackBoxSize.child("left");
	up = attackBoxSize.child("up");
	right = attackBoxSize.child("right");
	attackBoxSizes[Facing::DOWN] = Vector3(down.attribute("x").as_int(),
		down.attribute("y").as_int(), down.attribute("z").as_int());
	attackBoxSizes[Facing::LEFT] = Vector3(left.attribute("x").as_int(),
		left.attribute("y").as_int(), left.attribute("z").as_int());
	attackBoxSizes[Facing::UP] = Vector3(up.attribute("x").as_int(),
		up.attribute("y").as_int(), up.attribute("z").as_int());
	attackBoxSizes[Facing::RIGHT] = Vector3(right.attribute("x").as_int(),
		right.attribute("y").as_int(), right.attribute("z").as_int());

	assets = assetSet;
	weaponAssets = weaponSet;
}