#include "Camera.h"


Camera::Camera(int vwprtWdth, int vwprtHght) {

	zoom = 1.0f;

	viewportWidth = vwprtWdth;
	viewportHeight = vwprtHght;
	viewportCenter = Vector3(viewportWidth * .5, viewportHeight * .5, 0);

	

	cameraPosition = Vector2::Zero;
}


Camera::~Camera() {
}

void Camera::updateViewport(const Vector2& viewport, const Vector2& viewportPos, bool zoomToFit) {

	viewportWidth = viewport.x - viewportPos.x;
	viewportHeight = viewport.y - viewportPos.y;
	viewportPosition = viewportPos;
	viewportCenter = Vector3((viewportWidth) * .5 + viewportPosition.x,
		(viewportHeight) * .5 + viewportPosition.y, 0);

}


bool Camera::viewContains(const Vector2& point) {
	RECT* rect = viewportWorldBoundary();

	bool contains = rect->left < point.x && point.x < rect->right
		&& rect->top < point.y && point.y < rect->bottom;
	delete rect;
	return contains;
}

float Camera::getZoom() {
	return zoom;
}

void Camera::setZoomToResolution(int width, int height) {
	float xZoom = Globals::WINDOW_WIDTH / width;
	float yZoom = Globals::WINDOW_HEIGHT / height;

	if (xZoom < yZoom)
		zoom = xZoom;
	else
		zoom = yZoom;
}

void Camera::setZoom(float zoomTo) {
	zoom = zoomTo;
	if (zoom < 0.2f)
		zoom = 0.2f;
	else if (zoom > 2.5f)
		zoom = 2.5;
}

void Camera::adjustZoom(float amount) {

	zoom += amount;
	if (zoom < 0.75f)
		zoom = 0.75f;
	else if (zoom > 3.5f)
		zoom = 3.5;

}


void Camera::moveCamera(Vector2 cameraMovement, bool clampToArea) {
	cameraPosition += cameraMovement;
}


RECT* Camera::viewportWorldBoundary() {

	//Vector2* viewportCorner = screenToWorld(Vector2::Zero);
	/*Vector2* viewportBottomCorner =
		screenToWorld(Vector2(viewportWidth, viewportHeight));*/

	RECT* rect = new RECT{
		(int) viewportPosition.x, (int) viewportPosition.y,
		(int) (viewportWidth - viewportPosition.x),
		(int) (viewportHeight - viewportPosition.y)};

	/*delete viewportCorner;
	delete viewportBottomCorner;*/

	return rect;
}


void Camera::centerOn(Vector2 pos, bool clampToArea) {

	cameraPosition = pos;
}

Vector2& Camera::worldToScreen(Vector2 worldPosition) {
	return Vector2::Transform(worldPosition, translationMatrix());
}

Vector2& Camera::screenToWorld(Vector2 screenPosition) {

	return Vector2::Transform(screenPosition, translationMatrix().Invert());
}



Matrix Camera::translationMatrix() {
	// casting to int prevents filtering artifacts??
	return Matrix::CreateTranslation(-(int) cameraPosition.x, -(int) cameraPosition.y, 0)
		* Matrix::CreateRotationZ(rotation)
		* Matrix::CreateScale(zoom, zoom, 1)
		* Matrix::CreateTranslation(viewportCenter);
}
