#pragma once

#include "../DXTKGui/Controls/DynamicDialog.h"
#include "../GameObjects/PlayerCharacter.h"
#include "SelectionPointer.h"

class PlayerSlot;
class PCSelectDialog : public DynamicDialog {
public:
	PCSelectDialog();
	//~PCSelectDialog();

	void reset();

	void loadPC(shared_ptr<AssetSet> pcAssets);
	virtual void setDimensions(const Vector2& position, const Vector2& size) override;

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;


	virtual void textureDraw(SpriteBatch* batch) override;

	virtual void hide() override;

	//shared_ptr<Joystick> connectedJoystick;
	PlayerSlot*  playerSlot;
	PlayerCharacter* pcSelected;

	JoyData* _threadJoystickData;
private:

	unique_ptr<Sprite> portrait;
	unique_ptr<Sprite> magicKanji;
	unique_ptr<Sprite> magicEnglish;

	Vector2 portraitPos;
	Vector2 magicPos;
	Vector2 pointerPos;

	bool playerReady = false;


	unique_ptr<SelectionPointer> pointer;
};