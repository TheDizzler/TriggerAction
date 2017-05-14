#pragma once

#include "../Creature.h"

class BaddieData {
public:
	BaddieData();
	virtual ~BaddieData();


	void loadData(xml_node baddieDataNode, shared_ptr<AssetSet> assetSet);

	string type;
	shared_ptr<AssetSet> assets;
	unique_ptr<Hitbox> hitbox;

};


class Baddie : public Creature {
public:
	Baddie(BaddieData* baddieData);
	virtual ~Baddie();

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch);


	//virtual void setPosition(const Vector3& position) override;

private:

	shared_ptr<Animation> walkDown;
	shared_ptr<Animation> walkLeft;
	shared_ptr<Animation> walkUp;
	shared_ptr<Animation> walkRight;

	shared_ptr<Animation> attackDown;
	shared_ptr<Animation> attackLeft;
	shared_ptr<Animation> attackUp;
	shared_ptr<Animation> attackRight;

	shared_ptr<Animation> provoke;
	shared_ptr<Animation> surprise;
	shared_ptr<Animation> hit;

	//GraphicsAsset* asset;
};