#pragma once
#include "PlayerCharacter.h"


class HitEffect : public IElement3D {
public:
	HitEffect(const Vector3& position);
	virtual ~HitEffect();

	/* GraphicsAsset is not stored in HitEffect. */
	virtual void load(GraphicsAsset* const graphicsAsset);

	bool update(double deltaTime);
	virtual void draw(SpriteBatch* batch) override;


	virtual const int getWidth() const override;
	virtual const int getHeight() const override;

private:


	ComPtr<ID3D11ShaderResourceView> texture;
	RECT sourceRect;

	double timeLive = 0;

	UINT width;
	UINT height;
	SpriteEffects spriteEffect = SpriteEffects_None;

};

/** Don't really need this. Just made implementatino easier. */
class HitEffectManager {
public:

	void loadHitEffects(shared_ptr<AssetSet> weaponSet);

	void update(double deltaTime);
	void draw(SpriteBatch* bacth);

	void newEffect(Facing facing, const Vector3& position, USHORT hitNumber);

	vector<GraphicsAsset*> hitEffects[4];
private:
	vector<HitEffect> liveEffects;
};


class Chrono : public PlayerCharacter {
public:
	Chrono(shared_ptr<PlayerSlot> slot);
	virtual ~Chrono();

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;
private:
	virtual void initializeAssets() override;
	virtual void loadWeapon(shared_ptr<AssetSet> weaponSet,
		Vector3 weaponPositions[4]) override;
	virtual void attackUpdate(double deltaTime) override;
	void endAttack();
	virtual void startMainAttack() override;

	double waitingTime = 0;
	double CAN_CANCEL_COOLDOWN_TIME = .3;
	USHORT currentComboAttack = 0;
	void firstAttack();
	void secondAttackStart();
	void secondAttack();
	void thirdAttackStart();
	void thirdAttack();
	void fourthAttackStart();
	void fourthAttack(double deltaTime);
	int FOURTH_ATTACK_JUMP_HEIGHT = 20;
	double FOURTH_ATTACK_COOLDOWN_MAX = 1.0;
	double fourthAttackCooldownTime = 0;
	double fourthAttackDuration = .5;
	double moveTime = 0;
	bool yetAttacked = false;
	bool yetAttackedThird = false;
	bool yetFourthAttack = false;
	//bool finishedJump = false;
	/** Position vector for lerp movement. */
	//Vector3 moveStart;
	/** Position vectors for lerp movement. */
	//Vector3 moveEnd;



	//bool attackQueued = false;
	enum AttackCombo {
		NONE = -1, FIRST_ATTACK, SECOND_ATTACK, THIRD_ATTACK, FOURTH_ATTACK, AWAIT_INPUT
	};
	AttackCombo currentAttack = NONE;

	Hitbox attackBox;
	Vector3 attackBoxSizes[4];
	Vector2 attackBoxOffset = Vector2(12, 12);
	bool drawAttack = false;
	unique_ptr<RectangleFrame> attackFrame;

	HitEffectManager hitEffectManager;
};


