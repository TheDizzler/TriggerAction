#include "../../pch.h"
#include "Chrono.h"

Chrono::Chrono(shared_ptr<PlayerSlot> slot) : PlayerCharacter(slot) {
	loadWeapon(characterData->weaponAssets, characterData->weaponPositions);

	currentHP = maxHP = 70;
	currentMP = maxMP = 8;
	PWR = 5;
	HIT = 8;
	MAG = 5;
	SPD = 13;
	EV = 8;
	STAM = 8;
	MDEF = 2;

	// Cyclone(4TP) -> Fire Whirl (Lucca) -> Aura Whirl (Marle)
	// Slash  -> Fire Sword (Lucca) -> Ice Sword (Marle)
	// Lightning
	// Spincut
	// Lightning 2
	// Life
	// Confuse
	// Luminaire
}

Chrono::~Chrono() {
}

void Chrono::attackUpdate(double deltaTime) {
}

void Chrono::startMainAttack() {
}

void Chrono::loadWeapon(shared_ptr<AssetSet> weaponSet, Vector3 weaponPositions[4]) {
}
