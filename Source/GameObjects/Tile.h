#pragma once

#include "Tangible.h"


class TileAsset;
class Map;

class TileBase : public IElement3D, public Maskable {
public:
	virtual ~TileBase();
	/** Empty update. */
	virtual void update(double deltaTime);

	virtual void moveBy(const Vector3& moveVector) override;
	virtual void setPosition(const Vector3 & position) override;

	virtual void calculateShadow(Map* map);
};


/* A standard boring old tile. Probably a ground tile. */
class Tile : public TileBase {
public:
	Tile();
	virtual ~Tile();

	/* TileAsset is not stored in Tile. */
	virtual void load(TileAsset* const tileAsset);

	virtual void draw(SpriteBatch* batch) override;

	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
protected:
	ComPtr<ID3D11ShaderResourceView> texture;

	RECT sourceRect;
	UINT width;
	UINT height;

	/** Position of tile above ground. For floating objects.
		This is independant from hitbox z position. */
	int startZposition = 0;
};

class Trigger;
class TangibleTile : public Tile, public Tangible {
public:
	virtual ~TangibleTile();
	virtual void load(TileAsset* const tileAsset) override;

	/** Special on-hit effects of object. */
	virtual void takeDamage(int damage, bool showDamage = true);

	virtual void calculateShadow(Map* map) override;

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void moveBy(const Vector3& moveVector) override;
	virtual void setPosition(const Vector3& position) override;

	virtual const Hitbox* getHitbox() const override;

	/** In situation where a 3D tile is flat, calculate the depthLayer from top left. */
	bool isFlat = false;
private:
	unique_ptr<RectangleSprite> shadow;
};


class Trigger : public Tangible {
public:
	Trigger(int rowdata[6]);
	Trigger(const Trigger* copytrigger);
	virtual ~Trigger();

	void debugDraw(SpriteBatch* batch);
	virtual void moveBy(const Vector3& moveVector);
	virtual bool activateTrigger(Creature* creature) = 0;

	virtual void takeDamage(int damage, bool showDamage = true) override;
};

class VerticalStepTrigger : public Trigger {
public:
	VerticalStepTrigger(int rowdata[6]);
	VerticalStepTrigger(const VerticalStepTrigger* copyTrigger);
	virtual ~VerticalStepTrigger();

	virtual bool activateTrigger(Creature* creature) override;
};


class HorizontalStepTrigger : public Trigger {
public:
	HorizontalStepTrigger(int rowdata[6]);
	HorizontalStepTrigger(const HorizontalStepTrigger* copyTrigger);
	virtual ~HorizontalStepTrigger();

	virtual bool activateTrigger(Creature* creature) override;

	bool rightUp;
};