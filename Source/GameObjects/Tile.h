#pragma once
//#include "../Managers/MapManager.h"

#include "Tangible.h"


class TileAsset;


class TileBase : public IElement3D, public Maskable {
public:

	/** Empty update. */
	virtual void update(double deltaTime);

	virtual void moveBy(const Vector3& moveVector) override;
	virtual void setPosition(const Vector3 & position) override;

};


/* A standard boring old tile. Probably a ground tile. */
class Tile : public TileBase {
public:
	Tile();


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



};


class TangibleTile : public Tile, public Tangible {
public:

	virtual void load(TileAsset* const tileAsset) override;

	virtual bool checkCollisionWith(const Hitbox* hitbox) const override;

	virtual void moveBy(const Vector3& moveVector) override;
	virtual void setPosition(const Vector3& position) override;

	virtual const Hitbox* getHitbox() const override;


};