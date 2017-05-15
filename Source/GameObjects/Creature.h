#pragma once
#include "Tangible.h"

class Creature : public Tangible {
public:
	virtual ~Creature();

	virtual void update(double deltaTime) = 0;
	virtual void draw(SpriteBatch* batch) = 0;

	/** NOTE: Currently returns FALSE always! */
	virtual bool checkCollisionWith(const Hitbox* hitbox) const override;

	int getHeight() const;
	int getWidth() const;
	virtual const Hitbox* getHitbox() const override;
	virtual void moveBy(const Vector3& moveVector) override;
	virtual void setPosition(const Vector3& position) override;

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

	shared_ptr<AssetSet> assetSet;
	shared_ptr<Animation> currentAnimation;

	void loadAnimation(const pugi::char_t* name);


	int facing = RIGHT;

	float currentFrameDuration = 0;
	double currentFrameTime = -1;
	int currentFrameIndex = 0;

	/* Position is bottom-left corner of sprite. */
	Vector3 position;
	Vector2 origin;
	Color tint = DirectX::Colors::White.v;
	float rotation = 0.0f;
	float layerDepth = 0.1f;
	Vector2 scale = Vector2(1, 1);
	//SpriteEffects spriteEffects = SpriteEffects_None;
};