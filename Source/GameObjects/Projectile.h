#pragma once
#include "IElement3D.h"


class Projectile : public IElement3D {
public:
	Projectile();
	virtual ~Projectile();

	void loadBullet(shared_ptr<Animation> bullet);
	void loadHitEffect(shared_ptr<Animation> hitEffect);

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch) override;

	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
protected:

	//shared_ptr<Animation> projectileUp;
	shared_ptr<Animation> projectileLeft;

	shared_ptr<Animation> hitEffect;
};