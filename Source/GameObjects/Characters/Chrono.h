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

	double timeLive;

	UINT width;
	UINT height;
	SpriteEffects spriteEffect = SpriteEffects_None;

};

/** Don't really need this. Just made implementation easier. */
class HitEffectManager {
public:

	void loadHitEffects(AssetSet* weaponSet);

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
	virtual void loadWeapon(AssetSet* weaponSet, Vector3 weaponPositions[4]) override;

	virtual void startMainAttack() override;
	virtual void attackUpdate(double deltaTime) override;
	void endAttack();
	

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
	void slideAttack();
	int FOURTH_ATTACK_JUMP_HEIGHT = 20;
	double FOURTH_ATTACK_COOLDOWN_MAX = 1.0;
	double fourthAttackCooldownTime = 0;
	double fourthAttackDuration = .5;
	double moveTime = 0;
	bool yetAttacked = false;
	bool lastAttackHit = false;

	int heavyAttackDamage = 10;
	int mediumAttackDamage = 5;
	int lightAttackDamage = 2;
	
	/** Position vector for lerp movement. */
	//Vector3 moveStart;
	/** Position vectors for lerp movement. */
	//Vector3 moveEnd;



	//bool attackQueued = false;
	enum Attacks {
		NONE = -1, FIRST_ATTACK, SECOND_ATTACK, THIRD_ATTACK, FOURTH_ATTACK, AWAIT_INPUT,
		HEAVY_ATTACK, SLIDE_ATTACK
	};
	Attacks currentAttack = NONE;

	Hitbox attackBox;
	Vector3 attackBoxSizes[4];
	Vector2 attackBoxOffset = Vector2(12, 12);
	/** Debug flag for drawing attack area. */
	bool drawAttackBox = false;
	unique_ptr<RectangleFrame> attackFrame;

	HitEffectManager hitEffectManager;
};


