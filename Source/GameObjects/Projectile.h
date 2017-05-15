#pragma once
#include "Tangible.h"

class Projectile : public IElement3D {
public:
	Projectile(Vector3 weaponPositions[4]);
	virtual ~Projectile();

	void loadBullet(shared_ptr<Animation> bullet, GraphicsAsset* shadow);
	void loadHitEffect(shared_ptr<Animation> hitEffect);

	/** Returns immediately if not active. */
	virtual void update(double deltaTime);
	/** Returns immediately if not active. */
	virtual void draw(SpriteBatch* batch) override;

	virtual void fire(Facing direction, const Vector3& position);
	virtual void store();

	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
protected:

	Vector3 weaponPositions[4];
	//shared_ptr<Animation> projectileUp;
	shared_ptr<Animation> projectileLeft;
	GraphicsAsset* shadow;
	Vector3 shadowPosition;
	Vector2 shadowOrigin;
	float shadowRotation;

	shared_ptr<Animation> hitEffect;

	float currentFrameDuration = 0;
	double currentFrameTime = -1;
	int currentFrameIndex = 0;

	bool isActive = false;
};