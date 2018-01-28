#pragma once

#include "../../DXTKGui/Controls/DynamicDialog.h"
#include "../GameObjects/Characters/PlayerCharacter.h"
#include "SelectionPointer.h"


class PCCombatDialog : public DynamicDialog {
public:
	PCCombatDialog(GUIFactory* factory);
	virtual ~PCCombatDialog();

	void loadPC(CharacterData* characterData);
	virtual void setDimensions(const Vector2& position, const Vector2& size) override;


	virtual bool update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

private:
	Vector2 portraitPos;
	Vector2 mpPos;
	Vector2 hpPos;
	unique_ptr<SelectionPointer> pointer;
	unique_ptr<Sprite> miniPortrait;
	unique_ptr<Sprite> spriteH;
	unique_ptr<Sprite> spriteM;
	unique_ptr<Sprite> spriteP;
	unique_ptr<TextLabel> hpTextLabel;
	unique_ptr<TextLabel> mpTextLabel;

};