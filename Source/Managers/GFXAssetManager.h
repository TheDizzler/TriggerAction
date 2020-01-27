#pragma once

#include "../GameObjects/Characters/CharacterData.h"
#include "../GameObjects/Baddies/Baddie.h"

class GFXAssetManager {
public:
	GFXAssetManager(xml_node assetManifestRoot);
	~GFXAssetManager();

	bool initialize(ComPtr<ID3D11Device> device);

	unique_ptr<Sprite> getSpriteFromAsset(const char_t* assetName);
	/* Animations stored as unique_ptrs so no need to clean up. */
	Animation* getAnimation(const char_t* animationName);
	GraphicsAsset* const getAsset(const char_t* assetName);
	/* AssetSets stored as unique_ptrs so no need to clean up. */
	AssetSet* const getAssetSet(const char_t* setName);

	CharacterData* getNextCharacter(int* currentPCNum);
	CharacterData* getPreviousCharacter(int* currentPCNum);
	/* Currently only returns NULL. */
	CharacterData* getNextAvailabelCharacter();
	CharacterData* getCharacterData(string characterName);

	/** Returns true if character already selected. */
	bool setCharacterSelected(int currentPCNum, bool selected);

	unique_ptr<BaddieData> getBaddieData(
		ComPtr<ID3D11Device> device, string baddieName);
private:
	xml_node gfxAssetsNode;
	xml_node characterDataNode;
	xml_node baddieDataNode;

	int numPCsAvailable;
	vector<bool> pcSelected;

	CRITICAL_SECTION cs_selectingPC;
	map<string, unique_ptr<CharacterData>> characterDataMap;

	map<string, unique_ptr<GraphicsAsset> > assetMap;
	map<string, unique_ptr<Animation>> animationMap;
	map<string, unique_ptr<AssetSet> > setMap;

	bool getGFXAssetsFromXML(ComPtr<ID3D11Device> device);
	bool getCharacterDataFromXML(ComPtr<ID3D11Device> device);
	bool getSpriteSheetData(ComPtr<ID3D11Device> device, xml_node gfxNode, string assetDir);
};