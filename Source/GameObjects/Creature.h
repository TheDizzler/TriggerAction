#pragma once
#include "Tangible.h"

class Creature : public IElement3D, public Tangible {
public:
	Creature();
	virtual ~Creature();

	virtual void takeDamage(int damage);

	virtual void update(double deltaTime) = 0;
	virtual void draw(SpriteBatch* batch) = 0;

	/** NOTE: Currently returns FALSE always! */
	virtual bool checkCollisionWith(const Hitbox* hitbox) const override;

	virtual const int getHeight() const;
	virtual const int getWidth() const;
	virtual const Hitbox* getHitbox() const override;
	virtual void moveBy(const Vector3& moveVector);
	virtual void setPosition(const Vector3& position);

protected:
	/*
	const char_t* animations[4] = {"waiting", "moving",
	enum CreatureAnimation {
		WAITING_ANIM, MOVING_ANIM, ATTACKING_ANIM, HIT_ANIM
	};
	CreatureAnimation animation;*/
	enum CreatureAction {
		WAITING_ACTION, MOVING_ACTION, ATTACKING_ACTION, HIT_ACTION
	};
	CreatureAction action = WAITING_ACTION;
	bool canCancelAction = true;

	virtual void moveUpdate(double deltaTime);
	virtual void attackUpdate(double deltaTime) = 0;
	/** The update for when creature has been hit with an attack. */
	virtual void hitUpdate(double deltaTime);

	shared_ptr<AssetSet> assetSet;
	shared_ptr<Animation> currentAnimation;

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

	Sprite shadow;

	void loadAnimation(shared_ptr<Animation> animation);
	/** Finds animation in a map, therefore not efficient. */
	void loadAnimation(const pugi::char_t* name);


	Facing facing = RIGHT;

	ID3D11ShaderResourceView* currentFrameTexture;
	RECT currentFrameRect;
	Vector2 currentFrameOrigin;
	float currentFrameDuration = 0;
	double currentFrameTime = -1;
	int currentFrameIndex = 0;

};