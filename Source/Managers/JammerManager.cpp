#include "../pch.h"
#include "JammerManager.h"
#include "../Engine/GameEngine.h"

JammerManager::JammerManager() {
}

JammerManager::~JammerManager() {
}

void JammerManager::update(double deltaTime) {

	for (int i = 0; i < jammers.size(); ) {
		jammers[i]->update(deltaTime);
		if (jammers[i]->isDone()) {
			swap(jammers[i], jammers.back());
			jammers.pop_back();
			continue;
		}
		++i;
	}
}

void JammerManager::draw(SpriteBatch* batch) {

	for (auto& jam : jammers)
		jam->draw(batch);
}

void JammerManager::createJam(Vector3 position, int message, Color color) {

	wostringstream woo;
	woo << message;

	unique_ptr<LetterJammer> jammer;
	jammer.reset(guiFactory->createLetterJammer(
		Vector2(position.x, position.y), woo.str(), color));
	jammer->setEffect(make_unique<RPGDamageJammer>(5, 10, 2.5));
	jammer->setScale(Vector2(.5, .5));
	jammer->setLayerDepth(1);
	jammers.push_back(move(jammer));

}

void JammerManager::createJam(Vector3 position, wstring message, Color color) {

	unique_ptr<LetterJammer> jammer;
	jammer.reset(guiFactory->createLetterJammer(
		Vector2(position.x, position.y), message, color));
	jammer->setEffect(make_unique<ColorJammer>(2.5));
	jammer->setLayerDepth(1);
	jammers.push_back(move(jammer));
}
