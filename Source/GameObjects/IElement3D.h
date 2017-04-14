#pragma once

#include "../DXTKGui/BaseGraphics/IElement2D.h"


class IElement3D {
public:


	virtual void draw(SpriteBatch* batch) = 0;

	virtual const Vector3& getPosition() const;
	//virtual const Vector3& getOrigin() const;
	//virtual const Vector3& getScale() const;
	virtual const float getRotation() const;
	//virtual const Color& getTint() const;
	//virtual const float getAlpha() const;
	virtual const int getWidth() const = 0;
	virtual const int getHeight() const = 0;
	virtual const float getLayerDepth() const;
	virtual void moveBy(const Vector3& moveVector);
	virtual void setPosition(const Vector3& position);
	virtual void setOrigin(const Vector2& origin);
	virtual void setScale(const Vector2& scale);
	virtual void setRotation(const float rotation);
	//virtual void setTint(const XMFLOAT4 color);
	//virtual void setTint(const Color & color);
	//virtual void setTint(const XMVECTORF32 color);
	//virtual void setAlpha(const float alpha);
	virtual void setLayerDepth(const float depth);
	/** Sometimes overlapping tiles have the same y-coord but one should always be on top.
		Depth nudge makes sure this happens. */
	virtual void setLayerDepth(const float depth, const float depthNudge);
	
protected:
	Vector2 origin;
	Color tint;
	float rotation;
	Vector2 scale;
	float layerDepth;
	float depthNudge = 0;
	
	/* Position is bottom left of sprite! 
		x, y, and height above y (z). */
	Vector3 position;

	Vector2 drawPosition;

	

};