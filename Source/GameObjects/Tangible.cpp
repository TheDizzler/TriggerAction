#include "../pch.h"
#include "Tangible.h"


Hitbox::Hitbox() {
}

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

bool Hitbox::collisionZ(const Hitbox* other) const {
	return position.z < other->position.z + other->size.z
		&& position.z + size.z > other->position.z;
}

bool Hitbox::contains(const Vector2& point) const {
	return position.x < point.x && position.x + size.x > point.x
		&& position.y < point.y && position.y + size.y > point.y;
}





#include "../Engine/GameEngine.h"
Tangible::~Tangible() {
}


void Tangible::debugUpdate() {

	testFrame->update();
}

void Tangible::debugDraw(SpriteBatch* batch) {

	testFrame->draw(batch);
}

void Tangible::debugSetTint(const Color& color) {

	testFrame->setTint(color);
}

void Tangible::setHitbox(const Hitbox box) {

	hitbox = Hitbox(box);
	hitboxOffset = box.position;

#ifdef  DEBUG_HITBOXES
	testFrame.reset(guiFactory->createRectangleFrame(
		Vector2(hitbox.position.x, hitbox.position.y),
		Vector2(hitbox.size.x, hitbox.size.y)));
		testFrame->setAlpha(.5);
#endif //  DEBUG_HITBOXES
}


void Tangible::moveHitboxBy(const Vector3& moveVector) {

	hitbox.position += moveVector;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveVector;

#ifdef  DEBUG_HITBOXES
	testFrame->moveBy(Vector2(moveVector.x, moveVector.y));
#endif //  DEBUG_HITBOXES
}

void Tangible::setHitboxPosition(const Vector3& newPosition) {

	hitbox.position = newPosition + hitboxOffset;
	hitbox.position.x -= hitbox.size.x / 2;
	hitbox.position.y -= hitbox.size.y;
	for (const auto& subHB : subHitboxes)
		subHB->position = newPosition;

#ifdef  DEBUG_HITBOXES
	testFrame->setPosition(Vector2(hitbox.position.x, hitbox.position.y));
#endif //  DEBUG_HITBOXES
}



