#pragma once

#include "../../DXTKGui/BaseGraphics/Sprite.h"

class Joystick;
class SelectionPointer : public Sprite {
public:
	SelectionPointer();
	virtual ~SelectionPointer();

	void reset();

	virtual void update(double deltaTime);

	void setSelected(bool selected);

	virtual void setPosition(const Vector2& newPosition) override;
private:

	bool selectedPosition = false;
	int selectedOffset = 16;

	double selectingTime;

	Vector2 startPos;
};