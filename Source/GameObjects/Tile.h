#pragma once
//#include "../Managers/MapManager.h"

#include "Tangible.h"

//class Map;
class TileAsset;
//class Map::TileAsset;

class Tile : public IElement3D {
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

	virtual void load(TileAsset* const tileAsset);

	virtual bool checkCollisionWith(_In_ const Hitbox* hitbox) const;

	virtual void moveBy(const Vector3& moveVector) override;
	virtual void setPosition(const Vector3& position) override;

	virtual const Hitbox* getHitbox() const override;


};