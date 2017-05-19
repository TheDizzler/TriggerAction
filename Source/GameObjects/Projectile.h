#pragma once
#include "Creature.h"

class Projectile : public IElement3D {
public:
	Projectile(Creature* owner, Vector3 weaponPositions[4]);
	virtual ~Projectile();

	void loadBullet(shared_ptr<Animation> bullet, GraphicsAsset* shadow);
	void loadHitEffect(shared_ptr<Animation> hitEffect);

	/** Returns immediately if not active. */
	virtual void update(double deltaTime);
	/** Returns immediately if not active. */
	virtual void draw(SpriteBatch* batch) override;

	virtual void fire(Facing direction, const Vector3& position);
	virtual void store();

	virtual void moveBy(const Vector3& moveVector);
	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
protected:
	Creature* owner;
	const Hitbox* ownerBox;

	Vector3 weaponPositions[4];
	//shared_ptr<Animation> projectileUp;
	shared_ptr<Animation> projectileLeft;
	GraphicsAsset* shadow;
	Vector3 shadowPosition;
	Vector2 shadowOrigin;
	float shadowRotation;
	Facing direction = Facing::LEFT;

	shared_ptr<Animation> hitEffect;

	ID3D11ShaderResourceView* currentFrameTexture;
	RECT currentFrameRect;
	Vector2 currentFrameOrigin;
	float currentFrameDuration = 0;
	double currentFrameTime = -1;
	int currentFrameIndex = 0;

	bool isActive = false;
	float projectileSpeed;
	int damage;

	Hitbox ray;
	/** Frame length assumed 1/60 second. */
	double distanceDeltaPerFrame;
	Vector2 moveInOneFrame;

	void hit(Tangible* liveObject);
	bool isExploding = false;
};