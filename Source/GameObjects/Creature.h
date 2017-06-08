#pragma once
#include "Tangible.h"

class Creature : public IElement3D, public Tangible {
public:
	Creature();
	virtual ~Creature();

	//virtual void update(double deltaTime) = 0;
	virtual void draw(SpriteBatch* batch) = 0;


	/** NOTE: NOT USED! Currently returns FALSE always! */
	virtual bool checkCollisionWith(const Hitbox* hitbox) const override;

	virtual const int getHeight() const;
	virtual const int getWidth() const;
	virtual const Hitbox* getHitbox() const override;
	virtual void moveBy(const Vector3& moveVector);
	virtual void setPosition(const Vector3& position);

	string name;
	SHORT currentHP = 0, maxHP = 0;
	/** Total attack power: PWR + WPN damage. */
	USHORT ATKPWR = 0;
	/** Total defense power: EV + ARM. */
	USHORT DEFPWR = 0;
	/** Defense against magical attacks. */
	USHORT MDEF = 0;
	/** Amount given when Baddie killed or current exp of PC */
	USHORT EXP = 0;
	/** Amont given when Baddie killed or current TP of PC. */
	USHORT TP = 0;
	
protected:

	enum CreatureAction {
		WAITING_ACTION, MOVING_ACTION, ATTACKING_ACTION, HIT_ACTION, JUMP_ACTION,
		BLOCK_ACTION, DRAWING_ACTION
	};
	CreatureAction action = WAITING_ACTION;
	bool canCancelAction = true;

	bool isAlive = true;
	// use this whenever gravity is doing its business
	bool falling = false;
	Vector3 fallVelocity = Vector3::Zero;

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

	double timeSinceDeath = 0;
};