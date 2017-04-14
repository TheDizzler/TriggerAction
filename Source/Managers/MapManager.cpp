#include "../pch.h"
#include "MapManager.h"

//map<int, float> Map::layerDepthMap;
//int Map::tileWidth, Map::tileHeight;
double Map::depthPerTile;
double Map::depthPerPixel;



//#include "../GameObjects/Tile.h"
Map::Map() {
}

Map::~Map() {

	animationMap.clear();
	assetMap.clear();
	layerMap.clear();
	hitboxes.clear();
}


void Map::update(double deltaTime) {
	//layerMap["animated layer"]->update(deltaTime);
	for (const auto& layer : layerMap) {
		layer.second->update(deltaTime);
	}
}


void Map::draw(SpriteBatch* batch) {

	for (const auto& layer : layerMap) {
		layer.second->draw(batch);
	}
	/* layerMap["ground"]->draw(batch);
	layerMap["far field"]->draw(batch);
	layerMap["collision objects"]->draw(batch);
	layerMap["useable objects"]->draw(batch);
	layerMap["collision objects 2"]->draw(batch);
	layerMap["cover objects 2"]->draw(batch);
	layerMap["cover objects"]->draw(batch);
	layerMap["near field"]->draw(batch);
	layerMap["animated layer"]->draw(batch);*/


}

void Map::loadMapDescription(xml_node mapRoot) {

	mapWidth = mapRoot.attribute("width").as_int();
	mapHeight = mapRoot.attribute("height").as_int();
	tileWidth = mapRoot.attribute("tilewidth").as_int();
	tileHeight = mapRoot.attribute("tileheight").as_int();

	trueMapWidth = mapWidth*tileWidth;
	trueMapHeight = mapHeight*tileHeight;

	depthPerTile = (NEAREST_DEPTH - FURTHEST_DEPTH) / mapHeight;
	depthPerPixel = (NEAREST_DEPTH - FURTHEST_DEPTH) / trueMapHeight;
}



TileAsset::~TileAsset() {
	properties.clear();
	hitboxes.clear();
}

AnimationAsset::AnimationAsset(ComPtr<ID3D11ShaderResourceView> tex,
	vector<shared_ptr<Frame>> frames) :Animation(tex, frames) {
}

AnimationAsset::~AnimationAsset() {
	properties.clear();
	hitboxes.clear();
}

Map::Layer::~Layer() {
	tiles.clear();
	//animations.clear();
}

void Map::Layer::update(double deltaTime) {
	for (const auto& tile : tiles)
		tile->update(deltaTime);
}


void Map::Layer::draw(SpriteBatch* batch) {

	for (const auto& tile : tiles)
		tile->draw(batch);
}


MapParser::MapParser(ComPtr<ID3D11Device> dev) {

	device = dev;
}

#include "../Engine/GameEngine.h"
bool MapParser::parseMap(xml_node mapRoot, string mapsDir) {

	map = make_unique<Map>();
	map->loadMapDescription(mapRoot);
	if (!loadTileset(mapRoot, mapsDir)) {
		GameEngine::errorMessage(L"Loading Tileset Failed");
		return false;
	}
	if (!loadLayerData(mapRoot)) {
		GameEngine::errorMessage(L"Loading LayerData Failed");
		return false;
	}

	return true;
}

unique_ptr<Map> MapParser::getMap() {
	return move(map);
}

#include "../DXTKGui/StringHelper.h"
bool MapParser::loadTileset(xml_node mapRoot, string mapsDir) {

	for (xml_node tilesetNode : mapRoot.children("tileset")) {


		string fileStr = mapsDir + tilesetNode.child("image").attribute("source").as_string();

		int tileWidth = tilesetNode.attribute("tilewidth").as_int();
		int tileHeight = tilesetNode.attribute("tileheight").as_int();

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

		Vector2 size = Vector2(tileWidth, tileHeight);
		Vector2 origin = Vector2(0, tileHeight);

		size_t rows = tilesetNode.attribute("tilecount").as_int() / columns;
		USHORT gid = firstGid;

		for (size_t j = 0; j < rows * tileHeight; j += tileHeight) {
			for (size_t i = 0; i < columns * tileWidth; i += tileWidth) {


				shared_ptr<TileAsset> spriteAsset;
				spriteAsset.reset(new TileAsset());
				spriteAsset->loadAsPartOfSheet(mapAsset->getTexture(), Vector2(i, j), size, origin);

				map->assetMap[gid++] = move(spriteAsset);
			}
		}

		for (xml_node tileNode : tilesetNode.children("tile")) {

			gid = tileNode.attribute("id").as_int() + firstGid;
			shared_ptr<TileAsset> tile = map->assetMap[gid];
			xml_node propertiesNode = tileNode.child("properties");
			if (propertiesNode) {

				for (xml_node propertyNode : propertiesNode.children("property")) {
					string propertyname = propertyNode.attribute("name").as_string();

					if (propertyname.compare("hitbox") == 0) {
						string str;
						if (propertyNode.attribute("value"))
							str = propertyNode.attribute("value").as_string();
						else
							str = propertyNode.text().as_string();

						istringstream datastream(str);
						string line;
						vector<vector<int>> data;
						while (getline(datastream, line)) {
							line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
							if (line.length() <= 0)
								continue;

							int rowdata[5];

							size_t xloc = line.find("x=") + 2;
							size_t yloc = line.find("y=") + 2;
							size_t widthloc = line.find("width=") + 6;
							size_t heightloc = line.find("height=") + 7;
							size_t zloc = line.find("z") + 2;

							//wostringstream wss;

							//int x; int y; int width; int height; int z;
							string substr = line.substr(xloc);
							istringstream(substr) >> rowdata[0];
							substr = line.substr(yloc);
							istringstream(substr) >> rowdata[1];
							substr = line.substr(widthloc);
							istringstream(substr) >> rowdata[2];
							substr = line.substr(heightloc);
							istringstream(substr) >> rowdata[3];
							substr = line.substr(zloc);
							istringstream(substr) >> rowdata[4];

							unique_ptr<Hitbox> hitbox = make_unique<Hitbox>(rowdata);
							tile->hitboxes.push_back(move(hitbox));

							//wss << x << " " << y << " " << width << " " << height << " " << z << endl;
							//OutputDebugString(wss.str().c_str());



						}

					} else if (propertyname.compare("mask") == 0) {

						Vector2 mask;


						string str = propertyNode.attribute("value").as_string();

						istringstream datastream(str);
						string line;
						vector<vector<int>> data;
						while (getline(datastream, line)) {
							line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
							if (line.length() <= 0)
								continue;

							size_t xloc = line.find("x=") + 2;
							size_t yloc = line.find("y=") + 2;

							string substr = line.substr(xloc);
							istringstream(substr) >> mask.x;
							substr = line.substr(yloc);
							istringstream(substr) >> mask.y;

							mask.y *= -1;
							tile->mask = mask;

						}
					} else {
						// generic (not yet used?) properties
						if (propertyNode.attribute("value"))
							tile->properties[propertyNode.attribute("name").as_string()]
							= propertyNode.attribute("value").as_string();
						else
							tile->properties[propertyNode.attribute("name").as_string()]
							= propertyNode.text().as_string();
					}
				}
			}

			xml_node animNode = tileNode.child("animation");
			if (animNode) {
				vector<shared_ptr<Frame>> frames;
				
				for (xml_node frameNode : animNode.children("frame")) {

					USHORT frameID = frameNode.attribute("tileid").as_int() + firstGid;
					shared_ptr<TileAsset> frameTile = map->assetMap[frameID];
					RECT rect;
					rect.left = frameTile->getPosition().x;
					rect.top = frameTile->getPosition().y;
					rect.right = rect.left + frameTile->getWidth();
					rect.bottom = rect.top + frameTile->getHeight();
					shared_ptr<Frame> frame;
					float frameTime = frameNode.attribute("duration").as_float() / 1000;
					frame.reset(new Frame(rect, frameTime));
					frames.push_back(move(frame));
					
					// remove from assets (?)
					map->assetMap.erase(frameID); // (???) if there are any properties they will be lost!
				}


				shared_ptr<AnimationAsset> animationAsset;
				animationAsset.reset(new AnimationAsset(mapAsset->getTexture(), frames));
				animationAsset->mask = tile->mask;
				for (auto& hb : tile->hitboxes)
					animationAsset->hitboxes.push_back(move(hb));
				animationAsset->properties = tile->properties;

				// store animation in map then remove from assets
				map->animationMap[gid] = animationAsset;
				map->assetMap.erase(gid);  // no point in keeping it since the hitboxes are gone
			}
		}
	}
	return true;
}


bool MapParser::loadLayerData(xml_node mapRoot) {


	float layerDepthNudge = .01 * Map::depthPerPixel;

	for (xml_node layerNode : mapRoot.children("layer")) {

		layerDepthNudge += layerDepthNudge;
		xml_node dataNode = layerNode.child("data");
		string layerName = layerNode.attribute("name").as_string();


		unique_ptr<Map::Layer> layer = make_unique<Map::Layer>(layerName);


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
				Vector3 position = Vector3(col * map->tileWidth, row * map->tileHeight + map->tileHeight, 0);

				float layerDepth = 0;
				if (layerName.compare("ground") == 0)
					layerDepth = 0.06;
				else if (layerName.compare("background") == 0)
					layerDepth = 0;
				else if (layerName.compare("foreground") == 0)
					layerDepth = .91;
				else
					layerDepth = map->getLayerDepth(position.y);

				// look in animation map first for ID
				if (!map->animationMap[gid]) {
					if (!map->assetMap[gid]) {
						wostringstream wss;
						wss << "Cannot find gid [" << gid << "] in map assets.";
						GameEngine::showErrorDialog(wss.str(), L"Error finding tile sprite");
						return false;
					}

					TileAsset* tileAsset = map->assetMap[gid].get();

					if (tileAsset->hitboxes.size() <= 0) {
						unique_ptr<Tile> tile = make_unique<Tile>();
						tile->load(tileAsset);
						tile->setOrigin(Vector2(0, tile->getHeight()));
						tile->setLayerDepth(layerDepth);
						tile->setPosition(position);
						layer->tiles.push_back(move(tile));
					} else {
						unique_ptr<TangibleTile> tile = make_unique<TangibleTile>();
						tile->load(tileAsset);
						tile->setOrigin(Vector2(0, tile->getHeight()));
						tile->setLayerDepth(layerDepth);
						tile->setPosition(position);
						layer->tiles.push_back(move(tile));
					}
				} else {

					unique_ptr<AnimatedTile> anim = make_unique<AnimatedTile>();
					anim->load(map->animationMap[gid]);
					anim->setLayerDepth(layerDepth, layerDepthNudge);
					anim->setOrigin(Vector2(0, anim->getHeight()));
					anim->setPosition(position);
					layer->tiles.push_back(move(anim));
				}

			}
		}

		map->layerMap[layerName] = move(layer);
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


