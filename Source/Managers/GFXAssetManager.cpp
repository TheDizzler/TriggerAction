#include "../pch.h"
#include "GFXAssetManager.h"

GFXAssetManager::GFXAssetManager(xml_node assetManifestRoot) {

	characterDataNode = assetManifestRoot.child("characterData");
	baddieDataNode = assetManifestRoot.child("baddieData");
	gfxAssetsNode = assetManifestRoot.child("gfx");
}

GFXAssetManager::~GFXAssetManager() {
	DeleteCriticalSection(&cs_selectingPC);
	assetMap.clear();
	animationMap.clear();
	setMap.clear();
	characterDataMap.clear();
}

#include "../Engine/GameEngine.h"
bool GFXAssetManager::initialize(ComPtr<ID3D11Device> device) {

	if (!getGFXAssetsFromXML(device)) {
		GameEngine::showErrorDialog(
			L"Sprite retrieval from Asset Manifest failed.", L"Epic failure");
		return false;
	}

	if (!getCharacterDataFromXML(device)) {
		GameEngine::showErrorDialog(
			L"Character gfx retrieval from Asset Manifest failed.", L"Epic failure");
		return false;
	}
	return true;
}

#include <sstream>
unique_ptr<Sprite> GFXAssetManager::getSpriteFromAsset(const char_t* assetName) {

	GraphicsAsset* const asset = getAsset(assetName);
	if (asset == NULL)
		return NULL;
	unique_ptr<Sprite> sprite;
	sprite.reset(new Sprite());
	sprite->load(asset);
	return sprite;
}

shared_ptr<Animation> GFXAssetManager::getAnimation(const char_t* animationName) {

	if (animationMap.find(animationName) == animationMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset file: " << animationName << endl;
		ws << "Creating empty animation." << endl;
		ws << "Total animations: " << animationMap.count(animationName) << endl;
		OutputDebugString(ws.str().c_str());

		GraphicsAsset* gfxAsset = getAsset(animationName);
		if (gfxAsset == NULL)
			return NULL;

		// create one frame animation
		float frameTime = 10000;
		vector<shared_ptr<Frame>> frames;
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = gfxAsset->getWidth();
		rect.bottom = gfxAsset->getHeight();
		shared_ptr<Frame> frame;
		frame.reset(new Frame(rect, Vector2::Zero, frameTime));
		frames.push_back(move(frame));
		shared_ptr<Animation> animationAsset;
		animationAsset.reset(new Animation(gfxAsset->getTexture(), frames));
		animationMap[animationName] = animationAsset;

	}

	return animationMap[animationName];

}

GraphicsAsset* const GFXAssetManager::getAsset(const char_t* assetName) {

	if (assetMap.find(assetName) == assetMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset file: " << assetName << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}

	return assetMap[assetName].get();
}

shared_ptr<AssetSet> const GFXAssetManager::getAssetSet(const char_t* setName) {

	if (setMap.find(setName) == setMap.end()) {
		wostringstream ws;
		ws << "Cannot find asset set: " << setName << "\n";
		OutputDebugString(ws.str().c_str());
		return NULL;
	}

	return setMap[setName];
}





#include "../DXTKGui/StringHelper.h"
bool GFXAssetManager::getGFXAssetsFromXML(ComPtr<ID3D11Device> device) {

	string assetsDir =
		gfxAssetsNode.parent().attribute("dir").as_string();

	string gfxDir = assetsDir + gfxAssetsNode.attribute("dir").as_string();


	for (xml_node spriteNode = gfxAssetsNode.child("sprite"); spriteNode;
		spriteNode = spriteNode.next_sibling("sprite")) {


		string file_s = gfxDir + spriteNode.attribute("file").as_string();
		const char_t* file = file_s.c_str();
		const char_t* name = spriteNode.attribute("name").as_string();
		string check = name;

		Vector2 origin = Vector2(-1000, -1000);
		xml_node originNode = spriteNode.child("origin");
		if (originNode) {
			origin.x = originNode.attribute("x").as_int();
			origin.y = originNode.attribute("y").as_int();
		}

		unique_ptr<GraphicsAsset> gfxAsset;
		gfxAsset.reset(new GraphicsAsset());
		if (!gfxAsset->load(device, StringHelper::convertCharStarToWCharT(file), origin)) {
			wstringstream wss;
			wss << "Unable to load texture file: " << file << " in GFX Asset Manager.";
			//MessageBox(0, wss.str().c_str(), L"Critical error", MB_OK);
			GameEngine::errorMessage(wss.str(), L"Critical error");
			return false;
		}

		if (spriteNode.attribute("set")) {
			string setName = spriteNode.attribute("set").as_string();
			if (setMap.find(setName) == setMap.end()) {
				// new set
				setMap[setName] = make_shared<AssetSet>(setName.c_str());
			}
			setMap[setName]->addAsset(check, move(gfxAsset));

		} else
			assetMap[check] = move(gfxAsset);
	}

	if (!getSpriteSheetData(device, gfxAssetsNode, gfxDir)) {
		wostringstream wss;
		wss << L"Could not read spritesheet data " << gfxDir.c_str() << L"!";
		GameEngine::errorMessage(wss.str().c_str(), L"Fatal Read Error!");
		return false;

	}



	return true;
}


CharacterData* GFXAssetManager::getNextCharacter(int* currentPCNum) {

	if (++(*currentPCNum) > numPCsAvailable - 1)
		*currentPCNum = 0;

	return characterDataMap[characters[*currentPCNum]].get();
}

CharacterData* GFXAssetManager::getPreviousCharacter(int* currentPCNum) {

	if (--(*currentPCNum) < 0)
		*currentPCNum = numPCsAvailable - 1;

	return characterDataMap[characters[*currentPCNum]].get();
}


CharacterData* GFXAssetManager::getCharacterData(string characterName) {
	return characterDataMap[characterName].get();
}

unique_ptr<BaddieData> GFXAssetManager::getBaddieData(
	ComPtr<ID3D11Device> device, string baddieName) {

	unique_ptr<BaddieData> baddieData = make_unique<BaddieData>();

	for (xml_node baddieDataNode : baddieDataNode.children("baddie")) {
		string thisBaddie = baddieDataNode.attribute("name").as_string();
		if (thisBaddie == baddieName) {

			xml_document baddieDoc;
			string filename = baddieDataNode.attribute("file").as_string();
			string file = gfxAssetsNode.parent().attribute("dir").as_string() + filename;
			baddieDoc.load_file(file.c_str());

			xml_node baddieFileroot = baddieDoc.child("root");

			for (xml_node baddieNode : baddieFileroot.children("baddie")) {
				thisBaddie = baddieNode.attribute("name").as_string();
				if (thisBaddie == baddieName) {

						string assetsDir = baddieFileroot.attribute("dir").as_string();
						if (!getSpriteSheetData(device, baddieNode, assetsDir)) {
							wostringstream wss;
							wss << L"Could not read baddie spritesheet data in ";
							wss << file.c_str() << L" file!";
							GameEngine::errorMessage(wss.str().c_str(), L"Fatal Read Error!");
							return baddieData;
						}
					baddieData->loadData(baddieNode, getAssetSet(baddieName.c_str()));

					break;
				}
			}
			break;
		}
	}
	return baddieData;
}


bool GFXAssetManager::getCharacterDataFromXML(ComPtr<ID3D11Device> device) {

	string assetsDir = gfxAssetsNode.parent().attribute("dir").as_string();

	for (xml_node characterNode : characterDataNode.children()) {

		string name = characterNode.attribute("name").as_string();

		unique_ptr<xml_document> charDoc = make_unique<xml_document>();
		string file = assetsDir + characterNode.attribute("file").as_string();
		if (!charDoc->load_file(file.c_str())) {
			wostringstream wss;
			wss << L"Could not read " << file.c_str() << L" file!";
			GameEngine::errorMessage(wss.str().c_str(), L"Fatal Read Error!");
			return false;
		}

		xml_node characterDataRoot = charDoc->child("root");
		string assetsDir = characterDataRoot.attribute("dir").as_string();


		if (!getSpriteSheetData(device, characterDataRoot, assetsDir)) {
			wostringstream wss;
			wss << L"Could not read spritesheet data in " << file.c_str() << L" file!";
			GameEngine::errorMessage(wss.str().c_str(), L"Fatal Read Error!");
			return false;
		}

		unique_ptr<CharacterData> characterData = make_unique<CharacterData>(name);
		characterData->loadData(characterDataRoot,
			getAssetSet(characterNode.attribute("name").as_string()),
			getAssetSet(characterNode.attribute("weapon").as_string()));
		xml_node weaponIconNode = characterDataRoot.child("weaponMenuIcon");
		characterData->weaponType = weaponIconNode.attribute("name").as_string();
		characterDataMap[name] = move(characterData);
	}

	numPCsAvailable = characterDataMap.size();


	InitializeCriticalSection(&cs_selectingPC);

	return true;
}


bool GFXAssetManager::getSpriteSheetData(ComPtr<ID3D11Device> device,
	xml_node gfxNode, string assetDir) {


	for (xml_node spritesheetNode : gfxNode.children("spritesheet")) {


		string file_s = assetDir + spritesheetNode.attribute("file").as_string();
		const char_t* file = file_s.c_str();


		unique_ptr<GraphicsAsset> masterAsset;
		string masterAssetName = spritesheetNode.attribute("name").as_string();

		// check it masterAsset alread exists; if it does, reuse it.
		masterAsset = move(assetMap[masterAssetName]);
		if (masterAsset == NULL)
			masterAsset = make_unique<GraphicsAsset>();
		if (!masterAsset->load(device, StringHelper::convertCharStarToWCharT(file))) {
			return false;
		}


		// parse all animations from spritesheet
		for (xml_node animationNode = spritesheetNode.child("animation");
			animationNode; animationNode = animationNode.next_sibling("animation")) {

			const char_t* name = animationNode.attribute("name").as_string();

			vector<shared_ptr<Frame>> frames;

			float timePerFrame = animationNode.attribute("timePerFrame").as_float();

			if (animationNode.attribute("interval")) {
				int interval = animationNode.attribute("interval").as_int();
				xml_node spriteNode = animationNode.child("sprite");

				for (int i = 0; i < animationNode.attribute("num").as_int(); ++i) {
					RECT rect;
					rect.left = spriteNode.attribute("x").as_int()
						+ (spriteNode.attribute("width").as_int() + interval) * i;
					rect.top = spriteNode.attribute("y").as_int();
					rect.right = rect.left + spriteNode.attribute("width").as_int();
					rect.bottom = rect.top + spriteNode.attribute("height").as_int();
					Vector2 origin = Vector2(0, 0);
					xml_node originNode = spriteNode.child("origin");
					if (originNode) {
						origin.x = originNode.attribute("x").as_int();
						origin.y = originNode.attribute("y").as_int();
					}
					shared_ptr<Frame> frame;
					frame.reset(new Frame(rect, origin, timePerFrame));
					frames.push_back(move(frame));

				}

			} else {
				for (xml_node spriteNode = animationNode.child("sprite"); spriteNode;
					spriteNode = spriteNode.next_sibling("sprite")) {

					RECT rect;
					rect.left = spriteNode.attribute("x").as_int();
					rect.top = spriteNode.attribute("y").as_int();
					rect.right = rect.left + spriteNode.attribute("width").as_int();
					rect.bottom = rect.top + spriteNode.attribute("height").as_int();

					Vector2 origin = Vector2(0, 0);
					xml_node originNode = spriteNode.child("origin");
					if (originNode) {
						origin.x = originNode.attribute("x").as_int();
						origin.y = originNode.attribute("y").as_int();
					}

					shared_ptr<Frame> frame;
					if (spriteNode.attribute("frameTime"))
						frame.reset(new Frame(rect, origin,
							spriteNode.attribute("frameTime").as_float()));
					else
						frame.reset(new Frame(rect, origin, timePerFrame));
					frames.push_back(move(frame));

				}
			}

			shared_ptr<Animation> animationAsset;
			animationAsset.reset(new Animation(masterAsset->getTexture(), frames));
			if (animationNode.attribute("set")) {
				string setName = animationNode.attribute("set").as_string();
				if (setMap.find(setName) == setMap.end()) {
					// new set
					setMap[setName] = make_shared<AssetSet>(setName.c_str());
				}
				setMap[setName]->addAsset(name, animationAsset);

			} else if (spritesheetNode.attribute("set")) {
				string setName = spritesheetNode.attribute("set").as_string();
				if (setMap.find(setName) == setMap.end()) {
					// new set
					setMap[setName] = make_shared<AssetSet>(setName.c_str());
				}
				setMap[setName]->addAsset(name, animationAsset);
			} else
				animationMap[name] = animationAsset;
		}
		// parse all single sprites from spritesheet
		for (xml_node spriteNode : spritesheetNode.children("sprite")) {

			const char_t* name = spriteNode.attribute("name").as_string();
			// pos in spritesheet
			Vector2 position = Vector2(spriteNode.attribute("x").as_int(),
				spriteNode.attribute("y").as_int());
			// dimensions in spritesheet
			Vector2 size = Vector2(spriteNode.attribute("width").as_int(),
				spriteNode.attribute("height").as_int());

			Vector2 origin = Vector2(-1000, -1000);
			xml_node originNode = spriteNode.child("origin");
			if (originNode) {
				origin.x = originNode.attribute("x").as_int();
				origin.y = originNode.attribute("y").as_int();
			}

			unique_ptr<GraphicsAsset> spriteAsset = make_unique<GraphicsAsset>();
			spriteAsset->loadAsPartOfSheet(masterAsset->getTexture(), position, size, origin);

			if (spriteNode.attribute("set")) {
				string setName = spriteNode.attribute("set").as_string();
				if (setMap.find(setName) == setMap.end()) {
					// new set
					setMap[setName] = make_shared<AssetSet>(setName.c_str());
				}
				setMap[setName]->addAsset(name, move(spriteAsset));

			} else if (spritesheetNode.attribute("set")) {
				string setName = spritesheetNode.attribute("set").as_string();
				if (setMap.find(setName) == setMap.end()) {
					// new set
					setMap[setName] = make_shared<AssetSet>(setName.c_str());
				}
				setMap[setName]->addAsset(name, move(spriteAsset));
			} else
				assetMap[name] = move(spriteAsset);
		}
		assetMap[masterAssetName] = move(masterAsset);
	}
	return true;
}
/**** ***** GFXAssetManager END ***** ****/
