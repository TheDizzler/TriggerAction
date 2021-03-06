#include "../pch.h"
#include "SelectionPointer.h"
#include "../../DXTKGui/Controllers/Joystick.h"

const int POINTER_OFFSET = 16;
const float BOUNCE_AMPLITUDE = 4;
const float BOUNCE_VELOCITY = 8;
SelectionPointer::SelectionPointer() {
	layerDepth = 1;
}

SelectionPointer::~SelectionPointer() {
}

void SelectionPointer::reset() {
	selectingTime = 0;
	position = startPos;
	setSelected(0);
}


void SelectionPointer::update(double deltaTime) {

	if (!selectedPosition) {
		// bouncing animation
		selectingTime += deltaTime;
		position.x = startPos.x + BOUNCE_AMPLITUDE * sin(BOUNCE_VELOCITY * selectingTime);
	} else {

		selectingTime += deltaTime;
		position.x = startPos.x + BOUNCE_AMPLITUDE * sin(8*BOUNCE_VELOCITY * selectingTime);

	}
}

void SelectionPointer::setSelected(bool selected) {
	selectedPosition = selected;
}


void SelectionPointer::setPosition(const Vector2& newPosition) {

	startPos = newPosition;
	position = newPosition;
	//position.x -= POINTER_OFFSET/2;
	selectingTime = 0;
	//Sprite::setPosition(newPosition);
}
