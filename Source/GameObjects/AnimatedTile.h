#pragma once

#include "IElement3D.h"

/** Animated Tiles have no hitbox. Required? */
class AnimatedTile : public IElement3D {
public:
	
	AnimatedTile();
	~AnimatedTile();

	void load(shared_ptr<Animation> animation);

	virtual void update(double deltaTime);
	virtual void draw(SpriteBatch * batch) override;

	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
	const RECT getRect() const;
	//const Hitbox* getHitbox() const;
	

	

	//virtual void moveBy(const Vector2 & moveVector) override;
	//virtual void setPosition(const Vector2 & position) override;

	

	void reset();
	bool repeats = true;
	bool isAlive = true;
	
protected:
	shared_ptr<Animation> animation;
	int currentFrameIndex = -1;

	double currentFrameTime = 0;


	Vector2 position;

	Vector2 scale = Vector2(1, 1);
	//unique_ptr<Hitbox> hitbox;

};