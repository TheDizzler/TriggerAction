#pragma once

#include "Tile.h"

class AnimationAsset;

/** Animated Tiles have no hitbox.  */
class AnimatedTile : public TileBase {
public:
	
	AnimatedTile();
	~AnimatedTile();

	void load(shared_ptr<AnimationAsset> animation);

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch * batch) override;

	virtual const int getWidth() const override;
	virtual const int getHeight() const override;
	const RECT getRect() const;


	

	void reset();
	bool repeats = true;
	bool isAlive = true;
	
protected:
	shared_ptr<Animation> animation;
	int currentFrameIndex = -1;
	float currentFrameDuration = 0;
	double currentFrameTime = 0;


};