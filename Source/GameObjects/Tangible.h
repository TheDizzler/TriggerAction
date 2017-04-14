#pragma once

#include "IElement3D.h"

struct Hitbox {
	Hitbox(int rowdata[5]);
	Hitbox(const Hitbox* copybox);
	//~Hitbox();

	/** Only checks x and y for collision. */
	bool collision2d(const Hitbox* other) const;
	bool collision(const Hitbox* other) const;
	bool collisionZ(const Hitbox* other) const;

	/* !! Does not account for z height !! */
	bool contains(const Vector2& point) const;

	/* Position relative to bottom-left of sprite containing this hotbox.
		position(x, y, height above y)*/
	Vector3 position;
	Vector3 size; // (width, height, z)
};


/* A drawable asset with Hitboxes. */
class Tangible {
public:

	~Tangible();

	void debugUpdate(Vector2 moveAmount);
	void debugDraw(SpriteBatch* batch);

	virtual bool checkCollisionWith(const Hitbox* hitbox) const = 0;

	virtual const Hitbox* getHitbox() const = 0;
	virtual void moveBy(const Vector3& moveVector) = 0;
	virtual void setPosition(const Vector3& position) = 0;
protected:
	unique_ptr<Hitbox> hitbox;
	/** Hit tests should check if objects collide in x and y axii of main hitbox before
	checking any other hitboxes. */
	vector<unique_ptr<Hitbox> > subHitboxes;

	unique_ptr<RectangleFrame> testFrame;

};


class Maskable {
public:

	virtual void moveBy(const Vector3& moveVector) = 0;
	virtual void setPosition(const Vector3& position) = 0;

protected:
/** Position of mask relative to bottom-left of containing IElement. */
	Vector2 maskPosition = Vector2::Zero;

};