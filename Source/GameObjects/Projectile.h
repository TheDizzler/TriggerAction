#pragma once
#include "Creature.h"

interface Projectile : public IElement3D {
public:
	Projectile(Creature* owner, Vector3 weaponPositions[4]);
	virtual ~Projectile();


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
	shared_ptr<Animation> projectileLeft;
	GraphicsAsset* shadow;
	Vector3 shadowPosition;

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


	virtual void loadBullet(shared_ptr<Animation> bullet, GraphicsAsset* shadow) = 0;
	void loadHitEffect(shared_ptr<Animation> hitEffect);

	bool fineHitDetection(const Hitbox* hb);

	void hit(Tangible* liveObject);
	bool isExploding = false;
};

/** ***** Marle's Bows ***** **/
class BronzeBow : public Projectile {
public:
	BronzeBow(Creature* owner,
		shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]);
	virtual ~BronzeBow();

private:
	virtual void loadBullet(shared_ptr<Animation> bullet,
		GraphicsAsset* shadow) override;
};


/** ***** Lucca's Guns. ***** **/
class AirGun : public Projectile {
public:
	AirGun(Creature* owner,
		shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]);
	virtual ~AirGun();

private:
	virtual void loadBullet(shared_ptr<Animation> bullet,
		GraphicsAsset* shadow) override;
};