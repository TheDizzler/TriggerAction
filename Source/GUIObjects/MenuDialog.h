#pragma once

#include "../DXTKGui/Controls/DynamicDialog.h"
#include "../GameObjects/Characters/PlayerCharacter.h"
#include "SelectionPointer.h"

enum TitleItems {
	NEW_GAME, LOAD_GAME, OPTIONS, QUIT_GAME
};

enum PauseMenuItems {
	CONTINUE, SETTINGS, QUIT, RELOAD_XML
};

class MenuDialog : public DynamicDialog {
public:
	MenuDialog(GUIFactory* guiFactory);
	virtual ~MenuDialog();

	virtual void setText(wstring text) override;
	virtual void setDimensions(const Vector2& position, const Vector2& size) override;
	virtual void pairPlayerSlot(PlayerSlot* playerSlot);

	void clearSelections();
	void addSelection(wstring selection, bool enabled);


	virtual void update(double deltaTime) override;
	virtual void draw(SpriteBatch* batch) override;

	virtual void show() override;
	virtual void hide() override;

	USHORT getSelected();
	bool selectionMade = false;
private:

	PlayerSlot*  playerSlot;
	unique_ptr<SelectionPointer> pointer;
	vector<unique_ptr<TextLabel>> selections;
	vector<bool> selectionsActive;
	short selectedItem = 0;
	Vector2 pointerPos;
	double repeatDelayTime = REPEAT_DELAY;
	/* Blocks auto-selection if dialog opens while a button is being pressed. */
	bool released = false;

	Vector2 firstSelectionPosition;
};