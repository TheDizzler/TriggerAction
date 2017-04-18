#pragma once

#include "Dialog.h"


class ImageDialog : Dialog {
public:
	ImageDialog(bool movable, bool centerText);
	~ImageDialog();

	virtual void initializeControl(GUIFactory* factory,
		shared_ptr<MouseController> mouseController);

	void initialize(shared_ptr<AssetSet> assetSet,
		const pugi::char_t* font = "Default Font");


	virtual void setText(wstring text) override;

	/** FrameThickness has no effect. */
	virtual void setDimensions(const Vector2& position, const Vector2& size,
		const int frameThickness = 2) override;

	//virtual void update(double deltaTime);
	virtual void draw(SpriteBatch* batch);


	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;

private:
	const pugi::char_t* assets[9] = {
		"Top Left Corner", "Top Center", "Top Right Corner",
		"Center Left", "Center Right",
		"Bottom Left Corner", "Bottom Center", "Bottom Right Corner",
		"Middle"};
	shared_ptr<AssetSet> assetSet;
	vector<unique_ptr<Sprite>> bgSprites;
};