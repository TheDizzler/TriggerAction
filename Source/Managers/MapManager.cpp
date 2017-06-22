#include "../pch.h"
#include "MapManager.h"
#include "../Engine/GameEngine.h"
//#include "../GameObjects/Baddies/Baddie.h"

double Map::depthPerTile;
double Map::depthPerPixel;



Map::Map() {
}

Map::~Map() {

	animationMap.clear();
	assetMap.clear();
	//layerMap.clear();
}


void Map::update(double deltaTime) {


	for (const auto& layer : layers)
		layer->update(deltaTime);

	for (int i = 0; i < baddies.size(); ) {
		if (baddies[i]->update(deltaTime)) {
			vector<Tangible*>::iterator it =
				find(tangiblesAll.begin(), tangiblesAll.end(), baddies[i].get());
			tangiblesAll.erase(it);

			swap(baddies[i], baddies.back());
			baddies.pop_back();
			continue;
		}
		++i;
	}

#ifdef  DEBUG_HITBOXES
	for (const auto& trigger : triggers)
		trigger->debugUpdate();
#endif //  DEBUG_HITBOXES
}


void Map::draw(SpriteBatch* batch) {

	for (const auto& layer : layers)
		layer->draw(batch);


	for (const auto& baddie : baddies)
		baddie->draw(batch);

#ifdef  DEBUG_HITBOXES
	for (const auto& trigger : triggers)
		trigger->debugDraw(batch);
#endif //  DEBUG_HITBOXES
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


void Map::loadBaddieType(USHORT gid, unique_ptr<BaddieData> baddieData) {

	baddieDataMap[gid] = move(baddieData);
}

void Map::placeBaddie(xml_node objectNode) {
	USHORT gid = objectNode.attribute("gid").as_int();

	int zHeight = 0;
	for (xml_node propertyNode : objectNode.child("properties")) {
		string name = propertyNode.child("property").attribute("name").as_string();
		if (name.compare("z") == 0)
			zHeight = propertyNode.child("property").attribute("value").as_int();
	}

	Vector3 pos(objectNode.attribute("x").as_int(), objectNode.attribute("y").as_int(), zHeight);

	if (baddieDataMap[gid]->type == "Blue Imp") {
		unique_ptr<Baddie> baddie = make_unique<BlueImp>(baddieDataMap[gid].get());
		pos.x += baddie->getWidth() / 2;
		baddie->setPosition(pos);

		baddies.push_back(move(baddie));
	}
}


void Map::placeTrigger(xml_node objectNode) {

	string type = objectNode.attribute("name").as_string();
	if (type.compare("step") == 0) {
		int minZ = 0, maxZ = 20;
		for (xml_node propertyNode : objectNode.child("properties")) {
			string propertyName = propertyNode.attribute("name").as_string();
			if (propertyName.compare("maxZ") == 0)
				maxZ = propertyNode.attribute("value").as_int();
			else if (propertyName.compare("minZ") == 0)
				minZ = propertyNode.attribute("value").as_int();
		}
		int data[6] = {
			objectNode.attribute("x").as_int(), -objectNode.attribute("y").as_int(), minZ,
			objectNode.attribute("width").as_int(), objectNode.attribute("height").as_int(), maxZ
		};

		unique_ptr<VerticalStepTrigger> trigger = make_unique<VerticalStepTrigger>(data);
		triggers.push_back(move(trigger));

	} else if (type.compare("start") == 0) {

		int z = objectNode.child("properties").child("property").attribute("value").as_int();
		int data[6] = {
			objectNode.attribute("x").as_int(), -objectNode.attribute("y").as_int(), z,
			objectNode.attribute("width").as_int(), objectNode.attribute("height").as_int(), 1
		};

		start = make_unique<EventTrigger>(data);

	} else if (type.compare("exit") == 0) {

		int z = objectNode.child("properties").child("property").attribute("value").as_int();
		int data[6] = {
			objectNode.attribute("x").as_int(), -objectNode.attribute("y").as_int(), z,
			objectNode.attribute("width").as_int(), objectNode.attribute("height").as_int(), 1
		};

		triggers.push_back(make_unique<EventTrigger>(data));
	}
}


void Map::placeTileTrigger(TileAsset* tileAsset, Vector3 position) {

	size_t zperLoc = tileAsset->stepUpValue.find("zperx=");
	if (zperLoc == -1) {
		zperLoc = tileAsset->stepUpValue.find("zpery=");
		string zperyStr = tileAsset->stepUpValue.substr(zperLoc + 6);
		float zpery;
		istringstream(zperyStr) >> zpery;

		int minZ = position.z;
		float maxZ = tileAsset->hitboxes[0]->size.z;
		int width = tileAsset->hitboxes[0]->size.x;
		int height = tileAsset->hitboxes[0]->size.y;
		int x = position.x;
		int y = tileAsset->hitboxes[0]->position.y - position.y + height;

		int data[6] = {
			x, y, minZ,
			width, height, maxZ
		};

		unique_ptr<VerticalStepTrigger> trigger = make_unique<VerticalStepTrigger>(data);
		triggers.push_back(move(trigger));
	}
	string zperxStr = tileAsset->stepUpValue.substr(zperLoc + 6);
	float zperx;
	istringstream(zperxStr) >> zperx;

	int minZ = position.z;
	float maxZ = tileAsset->hitboxes[0]->size.z;
	int width = tileAsset->hitboxes[0]->size.x;
	int height = tileAsset->hitboxes[0]->size.y;
	int x = position.x;
	int y = tileAsset->hitboxes[0]->position.y - position.y + height;

	int data[6] = {
		x, y, minZ,
		width, height, maxZ
	};

	unique_ptr<HorizontalStepTrigger> trigger = make_unique<HorizontalStepTrigger>(data);
	trigger->rightUp = (zperx > 0);
	triggers.push_back(move(trigger));
}


vector<TileBase*> Map::getTilesAt(Vector3 position) {

	int row = position.y / tileHeight;
	int col = position.x / tileWidth;

	vector<TileBase*> stack;
	for (const auto& layer : layers) {
		TileBase* tile = layer->tiles[row][col].get();
		if (tile)
			stack.push_back(tile);
	}

	return stack;
}



TileAsset::~TileAsset() {
}

void TileAsset::stepUp(string stepVal) {
	stepUpValue = stepVal;
}



AnimationAsset::AnimationAsset(ComPtr<ID3D11ShaderResourceView> tex,
	vector<shared_ptr<Frame>> frames) :Animation(tex, frames) {
}

AnimationAsset::~AnimationAsset() {
}



Map::Layer::~Layer() {
}

void Map::Layer::update(double deltaTime) {

	for (const auto& tileRow : tiles)
		for (const auto& tile : tileRow)
			if (tile.get())
				tile->update(deltaTime);
}


void Map::Layer::draw(SpriteBatch* batch) {

	if (texturized)
		texturePanel->draw(batch);
	else
		for (const auto& tileRow : tiles)
			for (const auto& tile : tileRow)
				if (tile.get())
					tile->draw(batch);
}

unique_ptr<GraphicsAsset> Map::Layer::texturize() {
	texturized = true;
	return move(guiFactory->createTextureFromIElement2D(this, false));
}

void Map::Layer::makeTexture(float layerDepth) {
	texturePanel.reset(guiFactory->createPanel(true));
	unique_ptr<GraphicsAsset> texture = texturize();
	texturePanel->setTexture(move(texture));
	texturePanel->setLayerDepth(layerDepth);
}

void Map::Layer::textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device) {
	batch->Begin(SpriteSortMode_FrontToBack, CommonStates(device.Get()).NonPremultiplied());
	{
		for (const auto& tileRow : tiles)
			for (const auto& tile : tileRow)
				if (tile.get())
					tile->draw(batch);
	}
	batch->End();
}

void Map::Layer::setPosition(const Vector2& position) {
}

const Vector2& Map::Layer::getPosition() const {
	return Vector2::Zero;
}

const int Map::Layer::getWidth() const {
	return Globals::WINDOW_WIDTH;
}

const int Map::Layer::getHeight() const {
	return Globals::WINDOW_HEIGHT;
}





MapParser::MapParser(ComPtr<ID3D11Device> dev) {

	device = dev;
}

MapParser::~MapParser() {
}

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

		string name = tilesetNode.attribute("name").as_string();
		USHORT firstGid = tilesetNode.attribute("firstgid").as_int();
		USHORT gid = firstGid;

		if (name.compare("Baddies") == 0) {
			for (xml_node tileNode : tilesetNode.children("tile")) {

				string baddieType
					= tileNode.child("properties").child("property")
					.attribute("value").as_string();
				gid = tileNode.attribute("id").as_int() + firstGid;
				map->loadBaddieType(gid, gfxAssets->getBaddieData(device, baddieType));
			}
			continue;
		}
		string fileStr = mapsDir
			+ tilesetNode.child("image").attribute("source").as_string();
		size_t loc = fileStr.find(".png");
		fileStr.replace(loc, 4, ".dds");

		int tileWidth = tilesetNode.attribute("tilewidth").as_int();
		int tileHeight = tilesetNode.attribute("tileheight").as_int();

		const wchar_t* file = StringHelper::convertCharStarToWCharT(fileStr.c_str());

		unique_ptr<GraphicsAsset> mapAsset = make_unique<GraphicsAsset>();
		if (!mapAsset->load(device, file)) {
			wstringstream wss;
			wss << "Unable to load map texture file: " << file;
			GameEngine::showErrorDialog(wss.str(), L"Critical error");
			return false;
		}



		USHORT columns = tilesetNode.attribute("columns").as_int();

		Vector2 size = Vector2(tileWidth, tileHeight);
		Vector2 origin = Vector2(0, tileHeight);

		size_t rows = tilesetNode.attribute("tilecount").as_int() / columns;


		for (size_t j = 0; j < rows * tileHeight; j += tileHeight) {
			for (size_t i = 0; i < columns * tileWidth; i += tileWidth) {


				shared_ptr<TileAsset> spriteAsset;
				spriteAsset.reset(new TileAsset());
				spriteAsset->loadAsPartOfSheet(
					mapAsset->getTexture(), Vector2(i, j), size, origin);

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
							line.erase(remove_if(
								line.begin(), line.end(), isspace), line.end());
							if (line.length() <= 0)
								continue;

							int rowdata[6];

							size_t xloc = line.find("x=") + 2;
							size_t yloc = line.find("y=") + 2;
							size_t zloc = line.find("z=") + 2;
							size_t widthloc = line.find("width=") + 6;
							size_t heightloc = line.find("height=") + 7;
							size_t zheightloc = line.find("zHeight=") + 8;

							string substr = line.substr(xloc);
							istringstream(substr) >> rowdata[0];
							substr = line.substr(yloc);
							istringstream(substr) >> rowdata[1];
							substr = line.substr(zloc);
							istringstream(substr) >> rowdata[2];
							substr = line.substr(widthloc);
							istringstream(substr) >> rowdata[3];
							substr = line.substr(heightloc);
							istringstream(substr) >> rowdata[4];
							substr = line.substr(zheightloc);
							istringstream(substr) >> rowdata[5];

							unique_ptr<Hitbox> hitbox = make_unique<Hitbox>(rowdata);
							tile->hitboxes.push_back(move(hitbox));

						}

					} else if (propertyname.compare("mask") == 0) {

						Vector2 mask;


						string str = propertyNode.attribute("value").as_string();

						istringstream datastream(str);
						string line;
						vector<vector<int>> data;
						while (getline(datastream, line)) {
							line.erase(remove_if(
								line.begin(), line.end(), isspace), line.end());
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
					} else if (propertyname.compare("stepUp") == 0) {
						tile->stepUp(propertyNode.attribute("value").as_string());
					} else if (propertyname.compare("flat") == 0) {
						tile->isFlat = propertyNode.attribute("value").as_bool();
					} else if (propertyname.compare("zPos") == 0) {
						tile->zPosition = propertyNode.attribute("value").as_int();
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
					Vector2 origin = Vector2(0, 0);
					xml_node originNode = frameNode.child("origin");
					if (originNode) {
						origin.x = originNode.attribute("x").as_int();
						origin.y = originNode.attribute("y").as_int();
					}
					shared_ptr<Frame> frame;
					float frameTime = frameNode.attribute("duration").as_float() / 1000;
					frame.reset(new Frame(rect, origin, frameTime));
					frames.push_back(move(frame));

					// remove from assets (?)
					map->assetMap.erase(frameID); // (???) if there are any properties they will be lost!
				}


				shared_ptr<AnimationAsset> animationAsset;
				animationAsset.reset(
					new AnimationAsset(mapAsset->getTexture(), frames));
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
		bool texturize = false;
		float layerDepth = 0;


		string str = dataNode.text().as_string();

		istringstream datastream(str);
		string line;
		vector<vector<int>> data;
		while (getline(datastream, line)) {
			line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
			if (line.length() <= 0)
				continue;
			vector<int> row = split(line);
			data.push_back(row);
			vector<unique_ptr<TileBase>> tileRow = vector<unique_ptr<TileBase>>();
			for (int i = 0; i < row.size(); ++i)
				tileRow.push_back(unique_ptr<TileBase>());
			layer->tiles.push_back(move(tileRow));
		}

		for (int row = 0; row < map->mapHeight; ++row) {
			for (int col = 0; col < map->mapWidth; ++col) {

				int gid = data[row][col];
				if (gid <= 0)
					continue;
				Vector3 position = Vector3(
					col * map->tileWidth, row * map->tileHeight + map->tileHeight, 0);

				int zPlus = 0;

				
				if (layerName.compare("ground") == 0) {
					layerDepth = 0.06;
					texturize = true;
				} else if (layerName.compare("background") == 0) {
					layerDepth = 0;
					texturize = true;
				} else if (layerName.compare("foreground") == 0) {
					layerDepth = .91;
					texturize = true;
				} else if (layerName.compare("zLayer") == 0) {
					zPlus = layerNode.child("properties").child("property")
						.attribute("value").as_int();
					position.y += zPlus;
					position.z = zPlus;
					layerDepth = map->getLayerDepth(position.y);
					texturize = false;
				} else {
					layerDepth = map->getLayerDepth(position.y);
					texturize = false;
				}



				// look in animation map first for ID
				if (!map->animationMap[gid]) {
					if (!map->assetMap[gid]) {
						wostringstream wss;
						wss << "Cannot find gid [" << gid << "] in map assets.";
						GameEngine::showErrorDialog(
							wss.str(), L"Error finding tile sprite");
						return false;
					}

					TileAsset* tileAsset = map->assetMap[gid].get();

					if (tileAsset->hitboxes.size() <= 0) {
						unique_ptr<Tile> tile = make_unique<Tile>();
						tile->load(tileAsset);
						tile->setOrigin(Vector2(0, tile->getHeight()));
						tile->setLayerDepth(layerDepth);
						tile->setPosition(position);
						layer->tiles[row][col] = move(tile);

					} else {
						if (tileAsset->stepUpValue.size() > 0) {
							map->placeTileTrigger(tileAsset, position);

							unique_ptr<Tile> tile = make_unique<Tile>();
							tile->load(tileAsset);
							tile->setOrigin(Vector2(0, tile->getHeight()));
							tile->setLayerDepth(layerDepth);
							tile->setPosition(position);
							layer->tiles[row][col] = move(tile);
						} else {
							unique_ptr<TangibleTile> tile = make_unique<TangibleTile>();
							tile->load(tileAsset);
							tile->setOrigin(Vector2(0, tile->getHeight()));
							tile->setPosition(position);
							map->tangibles.push_back(tile.get());
							layer->tiles[row][col] = move(tile);
						}
					}
				} else {

					unique_ptr<AnimatedTile> anim = make_unique<AnimatedTile>();
					anim->load(map->animationMap[gid]);
					anim->setLayerDepth(layerDepth, layerDepthNudge);
					anim->setOrigin(Vector2(0, anim->getHeight()));
					anim->setPosition(position);
					layer->tiles[row][col] = move(anim);
				}

			}
		}

		if (texturize)
			layer->makeTexture(layerDepth);
		map->layers.push_back(move(layer));
	}


	for (xml_node objectGroupNode : mapRoot.children("objectgroup")) {

		string name = objectGroupNode.attribute("name").as_string();
		if (name.compare("enemies") == 0) {
			for (xml_node objectNode : objectGroupNode.children("object")) {
				map->placeBaddie(objectNode);
			}
		} else if (name.compare("triggers") == 0) {
			for (xml_node objectNode : objectGroupNode.children("object")) {
				map->placeTrigger(objectNode);
			}
		}
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



EventTrigger::EventTrigger(int rowdata[6]) : Trigger(rowdata) {
}

EventTrigger::EventTrigger(const EventTrigger* copyTrigger)
	: Trigger(copyTrigger) {
}

EventTrigger::~EventTrigger() {
}

bool EventTrigger::activateTrigger(Creature * creature) {
	return false;
}
