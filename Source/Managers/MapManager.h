#pragma once
#define NEAREST_DEPTH .9
#define FURTHEST_DEPTH .1

#include "../Managers/GFXAssetManager.h"
#include "../GameObjects/AnimatedTile.h"
#include "../GameObjects/Tile.h"


class Baddie;
class BaddieData;

class TileAsset : public GraphicsAsset {
public:
	virtual ~TileAsset();

	map<string, string> properties;
	vector<unique_ptr<Hitbox>> hitboxes;
	vector<unique_ptr<Trigger>> triggers;

	void stepUp(string stepUpValue);
	/* Sets where the layer depth should be calculated from. */
	Vector2 mask;
	bool isFlat = false;
	/** Z height above ground. For floating objects. */
	int zPosition = 0;
	/** If this tile acts like a step. */
	string stepUpValue = "";
};

class AnimationAsset : public Animation {
public:
	AnimationAsset(ComPtr<ID3D11ShaderResourceView> tex,
		vector<unique_ptr<Frame>> frames, string aniName);
	virtual ~AnimationAsset();

	map<string, string> properties;
	vector<unique_ptr<Hitbox>> hitboxes;

	/* Sets where the layer depth should be calculated from. */
	Vector2 mask;
};

class EventTrigger : public Trigger {
public:
	EventTrigger(int rowdata[6]);
	EventTrigger(const EventTrigger* copyTrigger);
	virtual ~EventTrigger();

	virtual bool activateTrigger(Creature * creature) override;
};

/** Depth ranges from 0.0f to 1.0f. The entire depth between .1 and .9 (inclusive) is where the
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
	(calculated from bottom-left of object) that will change layer depths.*/
class Map {
	friend class MapParser;
public:
	class Layer : public Texturizable {
	public:
		Layer(string layerName) : name(layerName) {
		}
		virtual ~Layer();

		string name;
		// not taking advantage of this so might want to revert to single in-line vector
		vector<vector<unique_ptr<TileBase>>> tiles;

		void update(double deltaTime);
		void draw(SpriteBatch* batch);

		void makeTexture(float layerDepth);
		virtual unique_ptr<GraphicsAsset> texturize() override;
		virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;
		virtual void setPosition(const Vector2& position) override;
		virtual const Vector2& getPosition() const override;
		virtual const int getWidth() const override;
		virtual const int getHeight() const override;
	private:
		bool texturized = false;
		unique_ptr<TexturePanel> texturePanel;
		
		/* Not doing anything. */
		virtual void forceRefresh() override;
	};

	Map();
	virtual ~Map();

	void update(double deltaTime);
	void draw(SpriteBatch* batch);

	void loadMapDescription(xml_node mapRoot);
	void loadBaddieType(USHORT tileId, unique_ptr<BaddieData> baddie);
	void placeBaddie(xml_node objectNode);
	void placeTrigger(xml_node objectNode);
	void placeTileTrigger(TileAsset* tileAsset, Vector3 position);

	int mapWidth, mapHeight;
	int tileWidth, tileHeight;
	int trueMapWidth, trueMapHeight;

	unique_ptr<EventTrigger> start;

	map<USHORT, unique_ptr<BaddieData>> baddieDataMap;
	map<USHORT, unique_ptr<AnimationAsset>> animationMap;
	map<USHORT, unique_ptr<TileAsset>> assetMap;

	vector<unique_ptr<Map::Layer>> layers;
	vector<unique_ptr<Baddie>> baddies;
	vector<unique_ptr<Trigger>> triggers;
	/* When an object appears on screen, register its hitbox (if it has one) here. */
	vector<TangibleTile*> tangibles;

	/* How much the depthLayer variable changes with a one tile shift. */
	static double depthPerTile;
	static double depthPerPixel;

	/* If use depthPerPixel we could skip a division. Use bottom left ypos of sprite. */
	static float getLayerDepth(float ypos) {
		return ypos * depthPerPixel + FURTHEST_DEPTH;
	}

	/** Currently not used (you should probably be using hitboxes!).
			Returns stack of tiles occupying position. */
	vector<TileBase*> getTilesAt(Vector3 position);
	vector<TileBase*> getTilesBetween(Vector3 topLeft, Vector3 bottomRight);

	void calculateShadows();
};

/* Creates a unique_ptr to a Map but does not keep the reference. */
class MapParser {
	friend class Hitbox;
public:
	MapParser(ComPtr<ID3D11Device> device);
	virtual ~MapParser();

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