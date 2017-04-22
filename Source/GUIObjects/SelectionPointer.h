#pragma once

#include "../DXTKGui/BaseGraphics/Sprite.h"


class SelectionPointer : public Sprite {
public:
	SelectionPointer();
	//~SelectionPointer();

	virtual void update(double deltaTime);

	virtual void setPosition(const Vector2& newPosition) override;
private:

	bool selectedPosition = false;
	int selectedOffset = 16;

	double selectingTime;

	Vector2 startPos;
};