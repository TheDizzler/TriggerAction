#pragma once

#include "../DXTKGui/Controls/DynamicDialog.h"

class PCSelectDialog : public DynamicDialog {
public:
	PCSelectDialog();
	//~PCSelectDialog();

	void loadPC(shared_ptr<AssetSet> pcAssets);
	virtual void setDimensions(const Vector2& position, const Vector2& size) override;
	

	virtual void draw(SpriteBatch* batch) override;


	virtual void textureDraw(SpriteBatch* batch) override;
private:

	unique_ptr<Sprite> portrait;
	unique_ptr<Sprite> magicKanji;
	unique_ptr<Sprite> magicEnglish;




};