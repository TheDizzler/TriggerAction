#pragma once


class AssetSet;
class Hitbox;

class CharacterData {
public:
	CharacterData(string name);
	virtual ~CharacterData();

	void loadData(xml_node characterDataNode,
		AssetSet* assetSet, AssetSet* weaponSet);

	string name;

	AssetSet* assets;
	AssetSet* weaponAssets;

	unique_ptr<Hitbox> hitbox;
	Vector3 weaponPositions[4];
	Vector3 attackBoxSizes[4];

	string weaponType;
};