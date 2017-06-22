#pragma once

#include "../DXTKGui/Controls/DynamicDialog.h"
#include "../GameObjects/Characters/PlayerCharacter.h"
#include "SelectionPointer.h"


class PlayerSlot;
class PCSelectDialog : public DynamicDialog {
public:
	PCSelectDialog(GUIFactory* guiFactory);
	virtual ~PCSelectDialog();

	void reset();


	virtual void setDimensions(const Vector2& position, const Vector2& size) override;
	void loadPC(CharacterData* characterData);

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;


	virtual void textureDraw(SpriteBatch* batch, ComPtr<ID3D11Device> device = NULL) override;

	virtual void hide() override;
	virtual void show() override;

	void setSelected(bool selected);
	void setReady(bool isReady);
private:

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