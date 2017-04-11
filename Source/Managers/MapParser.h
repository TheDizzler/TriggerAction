#pragma once

#include "../Managers/GFXAssetManager.h"
#include "../GameObjects/AnimatedSprite.h"

struct Map {
	/*class MapAsset : public GraphicsAsset {
	public:

		string name;
		USHORT firstGid;
		USHORT tilecount;
		USHORT columns;
	};*/

	class TileAsset : public GraphicsAsset {
	public:
		~TileAsset();

		map<string, string> properties;
	};

	class Layer {
	public:
		Layer(string layerName) : name(layerName) {
		}
		~Layer();

		string name;
		//vector<vector<int>> data;
		vector<unique_ptr<Sprite> > tiles;
		vector<unique_ptr<AnimatedSprite> > animations;

		void update(double deltaTime);
		void draw(SpriteBatch* batch);

		/* Depth ranges from 0.0f to 1.0f. One tile will be equal to 0.1f
			and will cycle between 0.1f and 0.9f (lower number is further back). This should
			create the effect of 3D and still be manageable.
			Reserved layer depths:
				0.0f background images
				0.01f ground tiles
				1.0f foreground images. */
		void setLayerDepth();
	};

	~Map();

	void update(double deltaTime);
	void draw(SpriteBatch* batch);


	void loadMapDescription(xml_node mapRoot);

	int mapWidth, mapHeight;
	int tileWidth, tileHeight;
	int trueMapWidth, trueMapHeight;

	//vector<MapAsset> mapAssets;

	map<USHORT, shared_ptr<Animation>> animationMap;
	map<USHORT, shared_ptr<TileAsset>> assetMap;
	map<string, unique_ptr<Map::Layer>> layerMap;


};


class MapParser {
public:
	MapParser(ComPtr<ID3D11Device> device);
	//~MapParser();


	bool parseMap(xml_node mapRoot, string mapsDir);

	unique_ptr<Map> getMap();
private:

	unique_ptr<Map> map;


	bool loadTileset(xml_node mapRoot, string mapsDir);
	// Currently only parses CSV encoded tmx files
	bool loadLayerData(xml_node mapRoot);

	vector<int> split(string line);

	ComPtr<ID3D11Device> device;
};