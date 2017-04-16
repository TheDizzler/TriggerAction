#include "../pch.h"
#include "GUIOverlay.h"

#include "../Engine/GameEngine.h"
GUIOverlay::GUIOverlay(vector<shared_ptr<Joystick>> joys) {

	joysticks = joys;

	controllerLostDialog = guiFactory->createDialog();
	controllerLostDialog->setTitle(L"Controller Lost!");
}

void GUIOverlay::update(double deltaTime, shared_ptr<MouseController> mouse) {
}

void GUIOverlay::draw(SpriteBatch * batch) {
}


void GUIOverlay::openLostControllerDialog(size_t playerNum) {
	
}
