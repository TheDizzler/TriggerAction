#include "../pch.h"
#include "CharacterData.h"

#include "../Engine/GameEngine.h"
#include "../Managers/GFXAssetManager.h"
CharacterData::CharacterData(string nm) {

	name = nm;
	assets = gfxAssets->getAssetSet(name.c_str());

}

CharacterData::~CharacterData() {
}


void CharacterData::loadData(xml_node characterDataNode,
	shared_ptr<AssetSet> assetSet, shared_ptr<AssetSet> weaponSet) {

	xml_node hitboxNode = characterDataNode.child("hitbox");

	int rowdata[5] = {
		hitboxNode.attribute("x").as_int(), hitboxNode.attribute("y").as_int(),
		hitboxNode.attribute("width").as_int(), hitboxNode.attribute("height").as_int(),
		hitboxNode.attribute("z").as_int()};

	hitbox = make_unique<Hitbox>(rowdata);

	assets = assetSet;
	weaponAssets = weaponSet;
}
