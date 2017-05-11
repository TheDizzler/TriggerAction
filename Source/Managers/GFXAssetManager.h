#pragma once

#include "../GameObjects/CharacterData.h"


class GFXAssetManager {
public:
	GFXAssetManager(xml_node assetManifestRoot);
	~GFXAssetManager();

	bool initialize(ComPtr<ID3D11Device> device);

	unique_ptr<Sprite> getSpriteFromAsset(const char_t* assetName);
	shared_ptr<Animation> getAnimation(const char_t* animationName);
	GraphicsAsset* const getAsset(const char_t* assetName);
	shared_ptr<AssetSet> const getAssetSet(const char_t* setName);

	CharacterData* getNextCharacter(int* currentPCNum);
	CharacterData* getPreviousCharacter(int* currentPCNum);
	//const CharacterData* getNextAvailabelCharacter();
	const CharacterData* getPlayerData(string characterName);
	

private:
	xml_node gfxAssetsNode;
	xml_node characterDataNode;

	int numPCsAvailable;
	size_t nextAvaiablePC = 0;
	CRITICAL_SECTION cs_selectingPC;
	map<string, unique_ptr<CharacterData>> characterDataMap;

	map<string, unique_ptr<GraphicsAsset> > assetMap;
	map<string, shared_ptr<Animation> > animationMap;
	map<string, shared_ptr<AssetSet> > setMap;


	bool getGFXAssetsFromXML(ComPtr<ID3D11Device> device);
	bool getCharacterDataFromXML(ComPtr<ID3D11Device> device);
	bool getSpriteSheetData(ComPtr<ID3D11Device> device, xml_node gfxNode, string assetDir);


};