#include "../pch.h"
#include "Tangible.h"


Hitbox::Hitbox(int rowdata[5]) {

	position = Vector3(rowdata[0], -rowdata[1], 0);
	size = Vector3(rowdata[2], rowdata[3], rowdata[4]);

}

Hitbox::Hitbox(const Hitbox* copybox) {

	position = copybox->position;
	size = copybox->size;
}

Hitbox::~Hitbox() {
}

bool Hitbox::collision2d(_In_ const Hitbox* other) const {

	if (position.x < other->position.x + other->size.x
		&& position.x + size.x > other->position.x
		&& position.y < other->position.y + other->size.y
		&& position.y + size.y > other->position.y) {

		return true;

	}

	return false;
}

bool Hitbox::collision(_In_ const Hitbox*  other) const {

	if (position.x < other->position.x + other->size.x
		&& position.x + size.x > other->position.x
		&& position.y < other->position.y + other->size.y
		&& position.y + size.y > other->position.y

		&& position.z < other->position.z + other->size.z
		&& position.z + size.z > other->position.z) {

		return true;

	}

	return false;
}

bool Hitbox::collisionZ(const Hitbox * other) const {
	return position.z < other->position.z + other->size.z
		&& position.z + size.z > other->position.z;
}

bool Hitbox::contains(const Vector2 & point) const {
	return position.x < point.x && position.x + size.x > point.x
		&& position.y < point.y && position.y + size.y > point.y;
}





Tangible::~Tangible() {
	subHitboxes.clear();
}

#include "../Engine/GameEngine.h"
void Tangible::debugUpdate(Vector2 moveAmount) {

	if (!testFrame.get())
		testFrame.reset(guiFactory->createRectangleFrame(
			Vector2(hitbox->position.x, hitbox->position.y), Vector2(hitbox->size.x, hitbox->size.y)));

	testFrame->update();
	testFrame->moveBy(moveAmount);
}
void Tangible::debugDraw(SpriteBatch* batch) {

	if (!testFrame.get())
		testFrame.reset(guiFactory->createRectangleFrame(
			Vector2(hitbox->position.x, hitbox->position.y), Vector2(hitbox->size.x, hitbox->size.y)));

	testFrame->draw(batch);
}
