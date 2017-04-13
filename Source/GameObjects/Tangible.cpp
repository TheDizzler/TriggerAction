#include "../pch.h"
#include "Tangible.h"


Hitbox::Hitbox(int rowdata[5]) {

	position = Vector3(rowdata[0], rowdata[1], 0);
	size = Vector3(rowdata[2], rowdata[3], rowdata[4]);

}

Hitbox::Hitbox(_In_ Hitbox* copybox) {

	position = copybox->position;
	size = copybox->size;
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

//void Tangible::moveBy(const Vector3 & moveVector) {
//
//	position += moveVector;
//	drawPosition.x += moveVector.x;
//	drawPosition.y += moveVector.y + moveVector.z;
//	hitbox->position += moveVector;
//	for (const auto& subHB : subHitboxes)
//		subHB->position += moveVector;
//}
//
//void Tangible::setPosition(const Vector3& newpos) {
//
//	Vector3 moveBy = position - newpos;
//	position = newpos;
//	drawPosition.x = position.x;
//	drawPosition.y = position.y + position.z;
//
//
//	hitbox->position += moveBy;
//	for (const auto& subHB : subHitboxes)
//		subHB->position += moveBy;
//}
