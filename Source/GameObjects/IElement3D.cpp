#include "../pch.h"
#include "IElement3D.h"





const Vector3& IElement3D::getPosition() const {
	return position;
}

#include "../Managers/MapManager.h"
void IElement3D::moveBy(const Vector3& moveVector) {

	position += moveVector;
	drawPosition.x += moveVector.x;
	drawPosition.y += moveVector.y - moveVector.z;
}


void IElement3D::setPosition(const Vector3& newpos) {

	position = newpos;
	drawPosition.x = position.x;
	drawPosition.y = (position.y - position.z);
}



void IElement3D::setRotation(const float rot) {
	rotation = rot;
}
const float IElement3D::getLayerDepth() const {
	return layerDepth;
}
void IElement3D::setLayerDepth(const float depth, const float nudge) {
	depthNudge = nudge;
	layerDepth = depth + depthNudge;
}

void IElement3D::setLayerDepth(const float depth) {
	layerDepth = depth + depthNudge;
}

const float IElement3D::getRotation() const {
	return rotation;
}


void IElement3D::setOrigin(const Vector2& org) {
	origin = org;
}
void IElement3D::setScale(const Vector2& scl) {
	scale = scl;
}

//const Vector2& Tile::getOrigin() const {
//	return origin;
//}
//
//const Vector2 & Tile::getScale() const {
//	return scale;
//}
//const Color & Tile::getTint() const {
//	return tint;
//}
//
//const float Tile::getAlpha() const {
//	return tint.w;
//
//void Tile::setTint(const XMFLOAT4 color) {
//}
//
//void Tile::setTint(const Color & color) {
//}
//
//void Tile::setTint(const XMVECTORF32 color) {
//}
//
//void Tile::setAlpha(const float alpha) {
//}