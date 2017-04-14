#pragma once
//#include "PlayerCharacter.h"

class AssetSet;
class Hitbox;

class CharacterData {
public:
	CharacterData(string name);
	//~CharacterData();

	void loadData(xml_node characterDataNode, shared_ptr<AssetSet> assetSet);


	string name;
	shared_ptr<AssetSet> assets;

	unique_ptr<Hitbox> hitbox;

};