#include "../../pch.h"
#include "Baddie.h"
#include "../../Managers/GameManager.h"
#include "../../Engine/GameEngine.h"


BaddieData::BaddieData() {
}

BaddieData::~BaddieData() {
}

void BaddieData::loadData(xml_node baddieDataNode, shared_ptr<AssetSet> assetSet) {

	type = baddieDataNode.attribute("name").as_string();
	xml_node hitboxNode = baddieDataNode.child("hitbox");

	int rowdata[6] = {
		hitboxNode.attribute("x").as_int(), hitboxNode.attribute("y").as_int(),
		hitboxNode.attribute("z").as_int(),
		hitboxNode.attribute("width").as_int(), hitboxNode.attribute("height").as_int(),
		hitboxNode.attribute("zHeight").as_int()
	};
	hitbox = make_unique<Hitbox>(rowdata);

	xml_node weapPosNode = baddieDataNode.child("weaponPosition");
	xml_node down = weapPosNode.child("down");
	xml_node left = weapPosNode.child("left");
	xml_node up = weapPosNode.child("up");
	xml_node right = weapPosNode.child("right");
	weaponPositions[Facing::DOWN] = Vector3(down.attribute("x").as_int(),
		down.attribute("y").as_int(), down.attribute("z").as_int());
	weaponPositions[Facing::LEFT] = Vector3(left.attribute("x").as_int(),
		left.attribute("y").as_int(), left.attribute("z").as_int());
	weaponPositions[Facing::UP] = Vector3(up.attribute("x").as_int(),
		up.attribute("y").as_int(), up.attribute("z").as_int());
	weaponPositions[Facing::RIGHT] = Vector3(right.attribute("x").as_int(),
		right.attribute("y").as_int(), right.attribute("z").as_int());

	xml_node attackBoxSize = baddieDataNode.child("attackBoxSizes");
	down = attackBoxSize.child("down");
	left = attackBoxSize.child("left");
	up = attackBoxSize.child("up");
	right = attackBoxSize.child("right");
	attackBoxSizes[Facing::DOWN] = Vector3(down.attribute("x").as_int(),
		down.attribute("y").as_int(), down.attribute("z").as_int());
	attackBoxSizes[Facing::LEFT] = Vector3(left.attribute("x").as_int(),
		left.attribute("y").as_int(), left.attribute("z").as_int());
	attackBoxSizes[Facing::UP] = Vector3(up.attribute("x").as_int(),
		up.attribute("y").as_int(), up.attribute("z").as_int());
	attackBoxSizes[Facing::RIGHT] = Vector3(right.attribute("x").as_int(),
		right.attribute("y").as_int(), right.attribute("z").as_int());


	assets = assetSet;
}


#include "../../Screens/LevelScreen.h"
Baddie::Baddie(BaddieData* data) {

	setHitbox(data->hitbox.get());

	threatRange = Vector3(40, 40, 8 + 16); // z = jump height + attackbox z
	jumpSpeed = 85;

	assetSet = data->assets;
	walkDown = assetSet->getAnimation("walk down");
	walkLeft = assetSet->getAnimation("walk left");
	walkUp = assetSet->getAnimation("walk up");
	walkRight = assetSet->getAnimation("walk right");

	attackDown = assetSet->getAnimation("attack down");
	attackLeft = assetSet->getAnimation("attack left");
	attackUp = assetSet->getAnimation("attack up");
	attackRight = assetSet->getAnimation("attack right");

	provoke = assetSet->getAnimation("provoke");
	surprise = assetSet->getAnimation("surprise");
	hit = assetSet->getAnimation("hit");

	shadow.load(assetSet->getAsset("shadow"));

	loadAnimation(walkLeft);
	currentFrameTexture = currentAnimation->texture.Get();


}

Baddie::~Baddie() {
}


bool Baddie::update(double deltaTime) {

	if (isAlive) {
		switch (action) {
			case CreatureAction::WAITING_ACTION:
				currentFrameTime += deltaTime;
				if (currentFrameTime >= currentFrameDuration) {
					if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
						currentFrameIndex = 0;
					}
					currentFrameTime = 0;
					currentFrameDuration
						= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
					currentFrameRect
						= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
					currentFrameOrigin
						= currentAnimation->animationFrames[currentFrameIndex]->origin;
				}

				if (!falling) {
				/** Opportunity-attack */
					for (const auto& pc : pcs) {
						Vector3 distance = pc->getHitbox()->position - hitbox.position;

						if (target == NULL || !target->isAlive) {
							//target = pc.get();
							//action = FOLLOWING_TARGET;
						}

						if (abs(distance.x) < threatRange.x && abs(distance.y) < threatRange.y) {
							// le petit attaque
							distance.z = 8;
							distance.Normalize();
							//startMainAttack(distance);
							break;
						}
					}
				}
				break;
			case CreatureAction::FOLLOWING_TARGET:

				if (target != NULL && !falling) {

					// check for opportunity attack
					for (const auto& pc : pcs) {
						Vector3 distance = pc->getHitbox()->position - hitbox.position;

						if (abs(distance.x) < threatRange.x && abs(distance.y) < threatRange.y) {
							distance.z = 8;
							distance.Normalize();
							//startMainAttack(distance);
							break;
						}

					}

					if (action == CreatureAction::ATTACKING_ACTION)
						break;



					Vector3 direction = position - target->getPosition();
					direction.z = 0;
					direction.Normalize();

					Facing newFacing;
					if (abs(direction.x) > abs(direction.y)) {
						// more horizontal than vertical
						if (direction.x < 0) {
							// going right
							newFacing = Facing::RIGHT;
						} else {
							// going left
							newFacing = Facing::LEFT;
						}
					} else {
						// more vertical than horizontal
						if (direction.y < 0) {
							// going down
							newFacing = Facing::DOWN;
						} else {
							// going up
							newFacing = Facing::UP;
						}
					}

					if (newFacing != facing) {
						switch (newFacing) {
							case Facing::UP:
								loadAnimation(walkUp);
								break;
							case Facing::DOWN:
								loadAnimation(walkDown);
								break;
							case Facing::LEFT:
								loadAnimation(walkLeft);
								break;
							case Facing::RIGHT:
								loadAnimation(walkRight);
								break;
						}
						facing = newFacing;
					}

					moveBy(direction * -moveRightSpeed * deltaTime);

					currentFrameTime += deltaTime;
					if (currentFrameTime >= currentFrameDuration) {
						if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
							currentFrameIndex = 0;
						}
						currentFrameTime = 0;
						currentFrameDuration
							= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
						currentFrameRect
							= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
						currentFrameOrigin
							= currentAnimation->animationFrames[currentFrameIndex]->origin;
					}

				} else
					action = CreatureAction::WAITING_ACTION;
				break;
			case CreatureAction::MOVING_ACTION:
				break;
			case CreatureAction::ATTACKING_ACTION:
				attackUpdate(deltaTime);
				if (!target->isAlive) {
					target = NULL;
					action = CreatureAction::WAITING_ACTION;
				}
				break;

			case CreatureAction::HIT_ACTION:
				hitUpdate(deltaTime);
				target = NULL;
				break;

		}

		if (falling) {
			fallVelocity += GRAVITY * deltaTime;
			moveBy(fallVelocity);
			if (position.z <= 0) {
				Vector3 newpos = position;
				newpos.z = 0;
				setPosition(newpos);
				fallVelocity.z = 0;
				moveVelocity.z = 0;
				falling = false;
			}
		}

#ifdef  DEBUG_HITBOXES
		debugUpdate();
#endif //  DEBUG_HITBOXES
	} else {
		timeSinceDeath += deltaTime;

		double percentDead = timeSinceDeath / TIME_TO_DIE;
		tint = Color::Lerp(startTint, Color(0, 0, 0, 0), percentDead);
		if (percentDead >= 1) {
			// finish off creature
			return true;
		}
	}

	return false;
}


void Baddie::draw(SpriteBatch* batch) {

	batch->Draw(currentFrameTexture, drawPosition,
		&currentFrameRect, tint, rotation,
		currentFrameOrigin, scale,
		SpriteEffects_None, layerDepth);

	shadow.draw(batch);

#ifdef  DEBUG_HITBOXES
	debugDraw(batch);
#endif //  DEBUG_HITBOXES
}


void Baddie::takeDamage(int damage, bool showDamage) {

	if ((currentHP -= damage) < 0) {
		currentHP = 0;
		isAlive = false;
		timeSinceDeath = 0;
		startTint = tint;
	}

	if (showDamage)
		LevelScreen::jammerMan.createJam(position, damage);

	action = CreatureAction::HIT_ACTION;
	canCancelAction = false;
	loadAnimation(hit);
}




BlueImp::BlueImp(BaddieData* baddieData) : Baddie(baddieData) {

	name = baddieData->type;
	currentHP = maxHP = 130;
	DEFPWR = 127;
	MDEF = 50;
	EXP = 2;
	TP = 1;
	GOLD = 12;

	weight = 50;

	memcpy(attackBoxSizes, baddieData->attackBoxSizes, sizeof(attackBoxSizes));

	attackBox.size = attackBoxSizes[facing];

#ifdef  DEBUG_HITBOXES
	attackFrame.reset(guiFactory->createRectangleFrame(
		Vector2(attackBox.position.x, attackBox.position.y),
		Vector2(attackBox.size.x, attackBox.size.y)));
#endif //  DEBUG_HITBOXES

	memcpy(weaponPositions, baddieData->weaponPositions, sizeof(weaponPositions));
}

BlueImp::~BlueImp() {
}

bool BlueImp::update(double deltaTime) {
#ifdef  DEBUG_HITBOXES
	attackFrame->update();
#endif //  DEBUG_HITBOXES
	return Baddie::update(deltaTime);

}

void BlueImp::draw(SpriteBatch* batch) {
	Baddie::draw(batch);

#ifdef  DEBUG_HITBOXES
	if (drawAttackBox)
		attackFrame->draw(batch);
#endif //  DEBUG_HITBOXES
}


void BlueImp::startMainAttack(Vector3 direction) {

	action = CreatureAction::ATTACKING_ACTION;
	canCancelAction = false;

	// get direction to attack in
	if (abs(direction.x) > abs(direction.y)) {
		// attack more horizontal than vertical
		if (direction.x < 0) {
			// attack going left
			facing = Facing::LEFT;
			loadAnimation(attackLeft);
		} else {
			// attack going right
			facing = Facing::RIGHT;
			loadAnimation(attackRight);
		}
	} else {
		// attack more vertical than horizontal
		if (direction.y < 0) {
			// attack going up
			facing = Facing::UP;
			loadAnimation(attackUp);
		} else {
			// attack going down
			facing = Facing::DOWN;
			loadAnimation(attackDown);
		}
	}


	// hit detection
	attackBox.size = attackBoxSizes[facing];
	attackBox.position = position + weaponPositions[facing];
	switch (facing) {
		case Facing::LEFT:
			attackBox.position.x -= (currentFrameOrigin.x);
			break;
			/*case Facing::DOWN:
			break;
			case Facing::RIGHT:
			break;
			case Facing::UP:
			break;*/
	}


	attackBox.position.y -= attackBox.size.y;

	moveVelocity = direction * jumpSpeed;

}


void BlueImp::attackUpdate(double deltaTime) {
	currentFrameTime += deltaTime;
	if (currentFrameTime >= currentFrameDuration) {
		if (++currentFrameIndex >= currentAnimation->animationFrames.size()) {
			// attack sequence completed
			canCancelAction = true;
			switch (facing) {
				case Facing::DOWN:
					loadAnimation(walkDown);
					break;
				case Facing::LEFT:
					loadAnimation(walkLeft);
					break;
				case Facing::UP:
					loadAnimation(walkUp);
					break;
				case Facing::RIGHT:
					loadAnimation(walkRight);
					break;
			}
			action = CreatureAction::FOLLOWING_TARGET;

#ifdef  DEBUG_HITBOXES
			drawAttackBox = false;
#endif //  DEBUG_HITBOXES
			return;
		}
		if (currentFrameIndex == 2)
			moveVelocity = Vector3::Zero;
		else if (currentFrameIndex == 3)
			falling = true;

		currentFrameTime = 0;
		currentFrameDuration
			= currentAnimation->animationFrames[currentFrameIndex]->frameTime;
		currentFrameRect
			= currentAnimation->animationFrames[currentFrameIndex]->sourceRect;
		currentFrameOrigin
			= currentAnimation->animationFrames[currentFrameIndex]->origin;
	}


	switch (currentFrameIndex) {
		case 0:
			hitList.clear();
			break;
		case 1:
		{
			// jump forward
			Vector3 moveAmount = moveVelocity * deltaTime;
			moveBy(moveAmount);
			attackBox.position += moveAmount;

#ifdef  DEBUG_HITBOXES
			drawAttackBox = true;
			attackFrame->setSize(Vector2(attackBox.size.x, attackBox.size.y));
			attackFrame->setPosition(Vector2(
				attackBox.position.x, attackBox.position.y));
#endif //  DEBUG_HITBOXES

			for (Tangible* object : hitboxesAll) {
				if (object == this) {
					continue;
				}
				if (std::find(hitList.begin(), hitList.end(), object) != hitList.end())
					continue;
				if (attackBox.collision(object->getHitbox())) {
					object->knockBack(moveVelocity*2, weight*2);
					object->takeDamage(5);
					hitList.push_back(object);
					// impact effect, if any
					//hitEffectManager.newEffect(facing, position, 0);
				}
			}
		}
		break;

		case 2: // hanging

			break;
		case 3: // falling
			//moveBy(moveVelocity);
			if (!falling) { // finish fall
				currentFrameTime = 10;
			} else
				currentFrameTime = 0;
			break;

	}
}