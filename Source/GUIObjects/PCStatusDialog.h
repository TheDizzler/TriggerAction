#pragma once
#include "../DXTKGui/Controls/DynamicDialog.h"
#include "../GameObjects/Characters/PlayerCharacter.h"

class PCStatusDialog : public DynamicDialog {
public:
	PCStatusDialog(GUIFactory* guiFactory);
	virtual ~PCStatusDialog();

	virtual void setDimensions(const Vector2& position, const Vector2& size) override;
	void loadPC(PlayerCharacter* pc);

	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	void updateHP();
	void updateMP();

private:

	PlayerCharacter* player;

	unique_ptr<TextLabel> nameLabel;
	unique_ptr<TextLabel> hpLabel;
	unique_ptr<TextLabel> mpLabel;

	Vector2 namePosition;
	Vector2 hpPosition;
	Vector2 mpPosition;
};