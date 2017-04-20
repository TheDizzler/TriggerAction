#pragma once

#include "Dialog.h"

/* This Dialog box requires an AssetSet with nine assets:
	"Top Left Corner",  "Top Center", "Top Right Corner",
	"Center Left", "Middle", "Center Right"
	"Bottom Left Corner", "Bottom Center", "Bottom Right Corner"
	This Dialog is expensive and should be used sparingly. */
class DynamicDialog : Dialog {
public:
	DynamicDialog(bool movable, bool centerText);
	~DynamicDialog();


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

	virtual void setPosition(const Vector2& newPosition) override;
	virtual void setLayerDepth(const float depth, bool frontToBack = true) override;


	virtual void show() override;
	virtual void close() override;

	virtual bool isShowing() override;
private:

	shared_ptr<AssetSet> assetSet;
	vector<unique_ptr<Sprite>> bgSprites;

	Vector2 bottomLeftPosition;
	
	struct miniAsset {


	};

	GraphicsAsset* topLeftCorner;
	GraphicsAsset* topCenter;
	GraphicsAsset* topRightCorner;

	GraphicsAsset* centerLeft;
	GraphicsAsset* middle;
	GraphicsAsset* centerRight;

	GraphicsAsset* bottomLeftCorner;
	GraphicsAsset* bottomCenter;
	GraphicsAsset* bottomRightCorner;
};