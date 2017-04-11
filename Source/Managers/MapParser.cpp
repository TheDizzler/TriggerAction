#include "../pch.h"
#include "MapParser.h"

MapParser::MapParser(ComPtr<ID3D11Device> dev) {

	device = dev;
}

bool MapParser::parseMap(xml_node mapRoot, string mapsDir) {

	map = make_unique<Map>();
	map->loadMapDescription(mapRoot);
	if (!loadTileset(mapRoot, mapsDir))
		return false;
	if (!loadLayerData(mapRoot))
		return false;

	return true;
}

unique_ptr<Map> MapParser::getMap() {
	return move(map);
}

Map::~Map() {

	//mapAssets.clear();
	animationMap.clear();
	assetMap.clear();
	layerMap.clear();
}


void Map::update(double deltaTime) {
	layerMap["animated layer"]->update(deltaTime);
}


void Map::draw(SpriteBatch* batch) {

	layerMap["ground"]->draw(batch);
	layerMap["far field"]->draw(batch);
	layerMap["collision objects"]->draw(batch);
	layerMap["useable objects"]->draw(batch);
	layerMap["collision objects 2"]->draw(batch);
	layerMap["cover objects 2"]->draw(batch);
	layerMap["cover objects"]->draw(batch);
	layerMap["near field"]->draw(batch);
	layerMap["animated layer"]->draw(batch);


}

void Map::loadMapDescription(xml_node mapRoot) {

	mapWidth = mapRoot.attribute("width").as_int();
	mapHeight = mapRoot.attribute("height").as_int();
	tileWidth = mapRoot.attribute("tilewidth").as_int();
	tileHeight = mapRoot.attribute("tileheight").as_int();

	trueMapWidth = mapWidth*tileWidth;
	trueMapHeight = mapHeight*tileHeight;
}

#include "../DXTKGui/StringHelper.h"
#include "../Engine/GameEngine.h"
bool MapParser::loadTileset(xml_node mapRoot, string mapsDir) {

	for (xml_node tilesetNode : mapRoot.children("tileset")) {

		//const char_t* file = tilesetNode.child("image").attribute("source").as_string();

		string fileStr = mapsDir + tilesetNode.child("image").attribute("source").as_string();

		const wchar_t* file = StringHelper::convertCharStarToWCharT(fileStr.c_str());

		unique_ptr<GraphicsAsset> mapAsset = make_unique<GraphicsAsset>();
		if (!mapAsset->load(device, file)) {
			wstringstream wss;
			wss << "Unable to load map texture file: " << file;
			//MessageBox(0, wss.str().c_str(), L"Critical error", MB_OK);
			GameEngine::showErrorDialog(wss.str(), L"Critical error");
			return false;
		}

		USHORT firstGid = tilesetNode.attribute("firstgid").as_int();
		string name = tilesetNode.attribute("name").as_string();
		USHORT columns = tilesetNode.attribute("columns").as_int();

		Vector2 size = Vector2(map->tileWidth, map->tileHeight);
		Vector2 origin = Vector2(0, 0);

		size_t rows = tilesetNode.attribute("tilecount").as_int() / columns;
		USHORT gid = firstGid;

		for (size_t j = 0; j < rows * map->tileHeight; j += map->tileHeight) {
			for (size_t i = 0; i < columns * map->tileWidth; i += map->tileWidth) {


				shared_ptr<Map::TileAsset> spriteAsset;
				spriteAsset.reset(new Map::TileAsset());
				spriteAsset->loadAsPartOfSheet(mapAsset->getTexture(), Vector2(i, j), size, origin);

				map->assetMap[gid++] = move(spriteAsset);
			}
		}

		for (xml_node tileNode : tilesetNode.children("tile")) {

			gid = tileNode.attribute("id").as_int() + firstGid;
			shared_ptr<Map::TileAsset> tile = map->assetMap[gid];
			xml_node propertiesNode = tileNode.child("properties");
			if (propertiesNode) {
				// get propertie info
				for (xml_node propertyNode : propertiesNode.children("property")) {
					tile->properties[propertyNode.attribute("name").as_string()]
						= propertyNode.attribute("value").as_string();
				}
			}

			xml_node animNode = tileNode.child("animation");
			if (animNode) {
				vector<shared_ptr<Frame>> frames;
				float frameTime;
				for (xml_node frameNode : animNode.children("frame")) {

					USHORT frameID = frameNode.attribute("tileid").as_int() + firstGid;
					shared_ptr<Map::TileAsset> frameTile = map->assetMap[frameID];
					RECT rect;
					rect.left = frameTile->getPosition().x;
					rect.top = frameTile->getPosition().y;
					rect.right = rect.left + frameTile->getWidth();
					rect.bottom = rect.top + frameTile->getHeight();
					shared_ptr<Frame> frame;
					frame.reset(new Frame(rect));
					frames.push_back(move(frame));
					frameTime = frameNode.attribute("duration").as_float() / 1000;
					// remove from assets (?)
					map->assetMap.erase(frameID); // (???) if there are any properties they will be lost!
				}

				
				shared_ptr<Animation> animationAsset;
				animationAsset.reset(new Animation(mapAsset->getTexture(), frames, frameTime));
				// store animation in map then remove from assets(?)
				map->animationMap[gid] = animationAsset;
				//map->assetMap.erase(gid); //(??) if there are any properties they will be lost!
			}
		}
	}
	return true;
}


bool MapParser::loadLayerData(xml_node mapRoot) {

	// will approx layerdepth by parsing order
	float layerDepth = 0;

	for (xml_node layerNode : mapRoot.children("layer")) {

		xml_node dataNode = layerNode.child("data");
		string name = layerNode.attribute("name").as_string();

		layerDepth = layerNode.child("properties").child("property").attribute("value").as_float();
		unique_ptr<Map::Layer> layer = make_unique<Map::Layer>(name);

		string str = dataNode.text().as_string();

		istringstream datastream(str);
		string line;
		vector<vector<int>> data;
		while (getline(datastream, line)) {
			line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
			if (line.length() <= 0)
				continue;
			data.push_back(split(line));
		}

		for (int row = 0; row < map->mapHeight; ++row) {
			for (int col = 0; col < map->mapWidth; ++col) {

				int gid = data[row][col];
				if (gid <= 0)
					continue;
				Vector2 position = Vector2(col*map->tileHeight, row*map->tileWidth);
				// look in animation map first for ID
				if (!map->animationMap[gid]) {
					if (!map->assetMap[gid]) {
						wostringstream wss;
						wss << "Cannot find gid [" << gid << "] in map assets.";
						GameEngine::showErrorDialog(wss.str(), L"Error finding tile sprite");
						return false;
					}

					unique_ptr<Sprite> sprite = make_unique<Sprite>();
					sprite->load(map->assetMap[gid].get());
					sprite->setPosition(position);
					sprite->setLayerDepth(layerDepth);
					layer->tiles.push_back(move(sprite));
				} else {

					unique_ptr<AnimatedSprite> anim = make_unique<AnimatedSprite>();
					anim->load(map->animationMap[gid]);
					anim->setLayerDepth(layerDepth);
					anim->setOrigin(Vector2::Zero);
					anim->setPosition(position);
					layer->animations.push_back(move(anim));
				}

			}
		}

		map->layerMap[name] = move(layer);
		//layerDepth += .05; // more than 20 (20!) layers will cause problems...should be ok
	}

	return true;
}


vector<int> MapParser::split(string line) {

	vector<int> rowdata;
	stringstream ss(line);
	string token;

	while (getline(ss, token, ',')) {
		int i;
		istringstream(token) >> i;
		rowdata.push_back(i);
	}

	return rowdata;

}


Map::TileAsset::~TileAsset() {
	properties.clear();
}

Map::Layer::~Layer() {
	tiles.clear();
	animations.clear();
}

void Map::Layer::update(double deltaTime) {
	for (const auto& anim : animations)
		anim->update(deltaTime);
}

void Map::Layer::draw(SpriteBatch* batch) {

	for (const auto& tile : tiles)
		tile->draw(batch);
	for (const auto& anim : animations)
		anim->draw(batch);

}
