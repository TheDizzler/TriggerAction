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

	virtual void pairPlayerSlot(PlayerSlot* playerSlot);


	void loadPC(CharacterData* characterData);
	virtual void setDimensions(const Vector2& position, const Vector2& size) override;

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;


	virtual void textureDraw(SpriteBatch* batch) override;

	virtual void hide() override;


	void setSelected(bool selected);
	void setReady(bool isReady);
private:

	PlayerSlot*  playerSlot;
	CharacterData* characterData;

	unique_ptr<Sprite> portrait;
	unique_ptr<Sprite> magicKanji;
	unique_ptr<Sprite> magicEnglish;
	unique_ptr<Sprite> weaponType;

	Vector2 portraitPos;
	Vector2 magicPos;
	Vector2 weaponTypePos;
	Vector2 pointerPos;
	Vector2 readyLabelPos;

	bool playerReady = false;
	bool showReadyLabel = false;

	unique_ptr<TextLabel> readyLabel;
	unique_ptr<SelectionPointer> pointer;
};