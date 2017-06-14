#pragma once

#include "IElement3D.h"

/* GRAVITY(0, 0, -7) */
const Vector3 GRAVITY(0, 0, -7);
/* GROUND_FRICTION = .9 */
const double GROUND_FRICTION = .9;

class Hitbox {
public:
	Hitbox(); // required for use as non-pointer
	Hitbox(int rowdata[6]);
	Hitbox(const Hitbox* copybox);
	virtual ~Hitbox();

	/** Only checks x and y for collision. */
	bool collision2d(const Hitbox* other) const;
	bool collision(const Hitbox* other) const;
	bool collisionZ(const Hitbox* other) const;

	/* !! Does not account for z height !! */
	bool contains(const Vector2& point) const;

//private:
	/* Position relative to bottom-left of sprite containing this hotbox.
		position(x, y, height above y)*/
	Vector3 position;
	Vector3 size; // (width, height, z)
};

enum Facing {
	LEFT, DOWN, RIGHT, UP
};

/* A drawable asset with Hitboxes. */
class Tangible {
public:
	virtual ~Tangible();


	void debugUpdate();
	void debugDraw(SpriteBatch* batch);
	void debugSetTint(const Color& color);

	void setHitbox(const Hitbox hitbox);

	virtual bool checkCollisionWith(const Hitbox* hitbox) const = 0;

	virtual const Hitbox* getHitbox() const = 0;
	void moveHitboxBy(const Vector3& moveVector);
	void setHitboxPosition(const Vector3& newPosition);

	virtual void takeDamage(int damage, bool showDamage = true) = 0;
	/** Do nothing for innanimate objects. Other objects should override */
	virtual void knockBack(Vector3 velocityOfHit, USHORT weightOfHit);
	/** Do nothing for innanimate objects. Other objects should override */
	virtual void knockBack(Vector3 moveVelocity);

protected:

	Hitbox hitbox;
	/** Hit tests should check if objects collide in x and y axii of main hitbox before
	checking any other hitboxes. */
	vector<unique_ptr<Hitbox> > subHitboxes;

	unique_ptr<RectangleFrame> testFrame;

	/** Used to calculate knockback. */
	USHORT weight = 1;
	//Vector3 knockBackVelocity = Vector3::Zero;
	/* use this whenever gravity is doing its business. */
	bool falling = false;

private:
	Vector3 hitboxOffset;
};


class Maskable {
public:

	virtual void moveBy(const Vector3& moveVector) = 0;
	virtual void setPosition(const Vector3& position) = 0;

protected:
/** Position of mask relative to bottom-left of containing IElement. */
	Vector2 maskPosition = Vector2::Zero;

};