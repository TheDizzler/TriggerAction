#include "../pch.h"
#include "Tangible.h"

#include "../Engine/GameEngine.h"


Hitbox::Hitbox() {
}

Hitbox::Hitbox(int rowdata[6]) {

	position = Vector3(rowdata[0], -rowdata[1], rowdata[2]);
	size = Vector3(rowdata[3], rowdata[4], rowdata[5]);
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





Tangible::~Tangible() {
}


void Tangible::debugUpdate() {

	testFrame->update();
	for (auto& subFrame : subTestFrames)
		subFrame->update();
}

void Tangible::debugDraw(SpriteBatch* batch) {

	testFrame->draw(batch);
	for (auto& subFrame : subTestFrames)
		subFrame->draw(batch);
}

void Tangible::debugSetTint(const Color& color) {

	testFrame->setTint(color, false);
}

void Tangible::setHitbox(const Hitbox box) {

	hitbox = Hitbox(box);
	hitboxOffset = box.position;

#ifdef  DEBUG_HITBOXES
	testFrame.reset(guiFactory->createRectangleFrame(
		Vector2(hitbox.position.x, hitbox.position.y),
		Vector2(hitbox.size.x, hitbox.size.y)));
	testFrame->setAlpha(.5);

	for (const auto& subs : subHitboxes) {
		unique_ptr<RectangleFrame> frame;
		frame.reset(guiFactory->createRectangleFrame(
			Vector2(subs->position.x, subs->position.y /*- subs->position.z*/),
			Vector2(subs->size.x, subs->size.y)));
		frame->setTint(Colors::Cornsilk);
		subTestFrames.push_back(move(frame));
	}
#endif //  DEBUG_HITBOXES
}

bool Tangible::checkCollisionWith(Tangible* other) {

	const Hitbox* otherBG = other->getHitbox();
	if (hitbox.collision2d(otherBG)) { // first check to see if hitbox overlap on x-y plane
		if (hitbox.collisionZ(otherBG)) // then check if collide on z-axis as well
			return true;
		for (const auto& subHB : subHitboxes) // then check inner hitboxes for collisions
			if (subHB->collision(otherBG))
				for (const auto& otherSubs : other->subHitboxes)
					if (subHB->collision(otherSubs.get()))
						return true;
	}
	return false;
}


const Hitbox* Tangible::getHitbox() const {
	return &hitbox;
}

void Tangible::moveHitboxBy(const Vector3& moveVector) {

	hitbox.position += moveVector;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveVector;

#ifdef  DEBUG_HITBOXES
	testFrame->moveBy(Vector2(moveVector.x, moveVector.y));
	for (auto& subFrame : subTestFrames)
		subFrame->moveBy(Vector2(moveVector.x, moveVector.y));
#endif //  DEBUG_HITBOXES
}

void Tangible::setHitboxPosition(const Vector3& newPosition) {

	Vector3 oldPos = hitbox.position;
	hitbox.position = newPosition + hitboxOffset;

	Vector3 moveVector = hitbox.position - oldPos;
	for (const auto& subHB : subHitboxes)
		subHB->position += moveVector;

#ifdef  DEBUG_HITBOXES
	testFrame->setPosition(Vector2(hitbox.position.x, hitbox.position.y));
	for (auto& subFrame : subTestFrames)
		subFrame->moveBy(Vector2(moveVector.x, moveVector.y));
#endif //  DEBUG_HITBOXES
}

void Tangible::knockBack(Vector3 velocityOfHit, USHORT weightOfHit) {

}

void Tangible::knockBack(Vector3 moveVelocity) {

}

Maskable::~Maskable() {
}
