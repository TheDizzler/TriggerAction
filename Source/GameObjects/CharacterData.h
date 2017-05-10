#pragma once


class AssetSet;
class Hitbox;

class CharacterData {
public:
	CharacterData(string name);
	virtual ~CharacterData();

	void loadData(xml_node characterDataNode, shared_ptr<AssetSet> assetSet);


	string name;
	shared_ptr<AssetSet> assets;

	unique_ptr<Hitbox> hitbox;

	string weaponType;
};