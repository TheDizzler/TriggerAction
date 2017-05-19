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

protected:
	virtual void attackUpdate(double deltaTime);

	Creature* target = NULL;
	void startMainAttack(Vector3 direction);
private:

	/** Range at which baddie will take an emergency action. */
	Vector3 threatRange;
	float jumpSpeed;
	Vector3 jumpVelocity;
};