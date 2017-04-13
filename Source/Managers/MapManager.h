#pragma once
#define NEAREST_DEPTH .9
#define FURTHEST_DEPTH .1

#include "../Managers/GFXAssetManager.h"
#include "../GameObjects/AnimatedTile.h"
#include "../GameObjects/Tile.h"
//#include "../GameObjects/Tangible.h"
//class Tile;
//class Hitbox;

class TileAsset : public GraphicsAsset {
public:
	~TileAsset();

	map<string, string> properties;
	vector<unique_ptr<Hitbox>> hitboxes;
};

class Map {
	friend class MapParser;
public:
	

	class Layer {
	public:
		Layer(string layerName) : name(layerName) {
		}
		~Layer();

		string name;
		
		vector<unique_ptr<Tile> > tiles;
		vector<unique_ptr<AnimatedTile> > animations;

		void update(double deltaTime);
		void draw(SpriteBatch* batch);

		/* Depth ranges from 0.0f to 1.0f. The entire depth between .1 and .9 (inclusive) is where the
				game action takes place. Layer depth is assigned to a live object (non-tile) by
				it's y-position. Objects at world co-ordinate (0, 0) have a layer depth of .1f.
				Objects at world coordinates (0, n) where n is the bottom most limit have a layer
				depth of .9f. Y-pos is always calculated by the sprites bottom-left corner.
			Reserved layer depths:
				0.0f <=	to	< 0.06f->	background images
				0.06f<=	to	< 0.1f ->	ground tiles
				0.1f <=	to	<=0.9f ->	all other tiles and sprites
				0.9f < 	to	<=1.0f ->	foreground images.
			WARNING: This method means that world y-coord 0 is absolute minimum for any object
				(calculated from bottom-left of object) that will change layer depths .*/
		void setLayerDepth();
	};

	Map();
	~Map();

	void update(double deltaTime);
	void draw(SpriteBatch* batch);


	void loadMapDescription(xml_node mapRoot);

	int mapWidth, mapHeight;
	/*static*/ int tileWidth, tileHeight;
	int trueMapWidth, trueMapHeight;



	map<USHORT, shared_ptr<Animation>> animationMap;
	map<USHORT, shared_ptr<TileAsset>> assetMap;
	
	map<string, unique_ptr<Map::Layer>> layerMap;
	/* When an object appears on screen, register its hitbox (if it has one) here. */
	vector<Hitbox> hitboxes;

	/* How much the depthLayer variable changes with a one tile shift. */
	static double depthPerTile;
	static double depthPerPixel;

	/* If use depthPerPixel we could skip a division. Use bottom left ypos of sprite. */
	static float getLayerDepth(int ypos) {

		//return (ypos / tileHeight) * depthPerTile + FURTHEST_DEPTH;
		return ypos * depthPerPixel + FURTHEST_DEPTH;
	}

	



};


class MapParser {
	friend class Hitbox;
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