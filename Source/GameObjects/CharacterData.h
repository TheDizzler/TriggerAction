#pragma once


class AssetSet;
class Hitbox;

class CharacterData {
public:
	CharacterData(string name);
	virtual ~CharacterData();

	void loadData(xml_node characterDataNode,
		shared_ptr<AssetSet> assetSet, shared_ptr<AssetSet> weaponSet);


	string name;
	shared_ptr<AssetSet> assets;
	shared_ptr<AssetSet> weaponAssets;

	unique_ptr<Hitbox> hitbox;

	string weaponType;
};