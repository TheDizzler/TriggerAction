#include "DynamicDialog.h"

DynamicDialog::DynamicDialog(bool canMove, bool centerTxt) {

	movable = canMove;
	centerText = centerTxt;
}

DynamicDialog::~DynamicDialog() {
	bgSprites.clear();
}

#include "GUIFactory.h"
void DynamicDialog::initializeControl(GUIFactory * factory,
	shared_ptr<MouseController> mouseController) {
	GUIControl::initializeControl(factory, mouseController);
}


void DynamicDialog::initialize(shared_ptr<AssetSet> set, const pugi::char_t* font) {

	assetSet = set;

	hitArea = make_unique<HitArea>(Vector2::Zero, Vector2::Zero);
	dialogText = make_unique<TextLabel>(guiFactory->getFont(font));
	//dialogText->setTint(Color(0, 0, 0, 1));

	setLayerDepth(.95);


}


void DynamicDialog::setText(wstring text) {
	dialogText->setText(text);
}

/** Using sprites appears to be slower. */
bool useSprites = false;
bool useTexture = true;
void DynamicDialog::setDimensions(const Vector2& posit, const Vector2& sz, const int frameThickness) {
	//for (const pugi::char_t* asset : assets) {

	size = sz;
	position = posit;
	bottomLeftPosition = position;
	bottomLeftPosition.y += size.y;

	if (useSprites) {

		Vector2 topPos = position;
		Vector2 bottomPos = bottomLeftPosition;
		unique_ptr<Sprite> sprite = make_unique<Sprite>();
		sprite->load(assetSet->getAsset("Top Left Corner"));
		sprite->setPosition(topPos);
		sprite->setOrigin(Vector2::Zero);
		bgSprites.push_back(move(sprite));


		sprite = make_unique<Sprite>();
		sprite->load(assetSet->getAsset("Bottom Left Corner"));
		sprite->setPosition(bottomPos);
		sprite->setOrigin(Vector2::Zero);
		int width = sprite->getWidth();
		int height;
		Vector2 cornerSize(width, sprite->getHeight());
		bgSprites.push_back(move(sprite));

		topPos = position + cornerSize;
		int maxLength = topPos.x + size.x - cornerSize.x * 2;
		int maxHeight = topPos.y + size.y - cornerSize.y * 2;
		while (topPos.y < maxHeight) {
			topPos.x = position.x + cornerSize.x;
			while (topPos.x < maxLength) {
				sprite = make_unique<Sprite>();
				sprite->load(assetSet->getAsset("Middle"));
				sprite->setPosition(topPos);
				sprite->setOrigin(Vector2::Zero);
				topPos.x += sprite->getWidth();
				height = sprite->getHeight();
				bgSprites.push_back(move(sprite));

			}
			topPos.y += height;
		}

		topPos = position;
		maxLength = position.x + size.x - width;
		topPos.x += width;
		bottomPos.x += width;
		while (topPos.x < maxLength) {

			sprite = make_unique<Sprite>();
			sprite->load(assetSet->getAsset("Top Center"));
			sprite->setPosition(topPos);
			sprite->setOrigin(Vector2::Zero);
			bgSprites.push_back(move(sprite));

			sprite = make_unique<Sprite>();
			sprite->load(assetSet->getAsset("Bottom Center"));
			sprite->setPosition(bottomPos);
			sprite->setOrigin(Vector2::Zero);
			width = sprite->getWidth();
			bgSprites.push_back(move(sprite));

			topPos.x += width;
			bottomPos.x += width;
		}



		sprite = make_unique<Sprite>();
		sprite->load(assetSet->getAsset("Top Right Corner"));
		sprite->setPosition(topPos);
		sprite->setOrigin(Vector2::Zero);
		bgSprites.push_back(move(sprite));


		sprite = make_unique<Sprite>();
		sprite->load(assetSet->getAsset("Bottom Right Corner"));
		sprite->setPosition(bottomPos);
		sprite->setOrigin(Vector2::Zero);
		int w = sprite->getWidth();
		height = sprite->getHeight();
		bgSprites.push_back(move(sprite));


		// draw verticals
		Vector2 leftPos = position;
		Vector2 rightPos = position;
		rightPos.x += size.x - w;


		maxHeight = position.y + size.y - height;
		leftPos.y += height;
		rightPos.y += height;
		while (leftPos.y < maxHeight) {

			sprite = make_unique<Sprite>();
			sprite->load(assetSet->getAsset("Center Left"));
			sprite->setPosition(leftPos);
			sprite->setOrigin(Vector2::Zero);
			bgSprites.push_back(move(sprite));


			sprite = make_unique<Sprite>();
			sprite->load(assetSet->getAsset("Center Right"));
			sprite->setPosition(rightPos);
			sprite->setOrigin(Vector2::Zero);
			height = sprite->getHeight();
			bgSprites.push_back(move(sprite));


			leftPos.y += height;
			rightPos.y += height;
		}

	} else {
		topLeftCorner = assetSet->getAsset("Top Left Corner");
		topCenter = assetSet->getAsset("Top Center");
		topRightCorner = assetSet->getAsset("Top Right Corner");

		centerLeft = assetSet->getAsset("Center Left");
		middle = assetSet->getAsset("Middle");
		centerRight = assetSet->getAsset("Center Right");

		bottomLeftCorner = assetSet->getAsset("Bottom Left Corner");
		bottomCenter = assetSet->getAsset("Bottom Center");
		bottomRightCorner = assetSet->getAsset("Bottom Right Corner");
	}

	if (size.x > 0 || size.y > 0) {
		panel.reset(guiFactory->createPanel());
		panel->setTint(Color(0, 1, 1, 1));
		Vector2 fix(-20, -20);
		panel->setDimensions(position/* + fix*/, size);
		isOpen = true;
		bool original = useTexture;
		useTexture = false;
		panel->setTexture(
			guiFactory->createTextureFromIElement2D(this, panel->getTint()));
		useTexture = original;
		isOpen = false;

		panel->setTexturePosition(position /*+ fix*/);
	}

	Vector2 textPos = position + dialogTextMargin;
	dialogText->setPosition(textPos);


}

void DynamicDialog::draw(SpriteBatch* batch) {

	/*if (!isOpen)
		return;*/

	if (useTexture) {
		panel->draw(batch);
	} else {
		if (useSprites) {
			for (const auto& sprite : bgSprites) {
				sprite->draw(batch);
			}

		} else {
			Vector2 topPos = position;
			Vector2 bottomPos = bottomLeftPosition;

			// draw left corners
			batch->Draw(topLeftCorner->getTexture().Get(), topPos, &topLeftCorner->getSourceRect(),
				tint, rotation, origin, scale, SpriteEffects_None, layerDepth);

			batch->Draw(bottomLeftCorner->getTexture().Get(), bottomPos,
				&bottomLeftCorner->getSourceRect(), tint, rotation, origin, scale, SpriteEffects_None, layerDepth);

			// draw middle
			int width = topLeftCorner->getWidth();
			Vector2 cornerSize(width, topLeftCorner->getHeight());
			topPos = position + cornerSize;
			int maxLength = topPos.x + size.x - cornerSize.x * 2;
			int maxHeight = topPos.y + size.y - cornerSize.y * 2;
			while (topPos.y < maxHeight) {
				topPos.x = position.x + cornerSize.x;
				while (topPos.x < maxLength) {
					batch->Draw(middle->getTexture().Get(), topPos,
						&middle->getSourceRect(), tint, rotation, origin, scale, SpriteEffects_None, layerDepth);

					topPos.x += middle->getWidth();
				}
				topPos.y += middle->getHeight();
			}

			topPos = position;
			maxLength = position.x + size.x - width;
			topPos.x += width;
			bottomPos.x += width;

			while (topPos.x < maxLength) {

				batch->Draw(topCenter->getTexture().Get(), topPos,
					&topCenter->getSourceRect(), tint, rotation, origin, scale, SpriteEffects_None, layerDepth);

				batch->Draw(bottomCenter->getTexture().Get(), bottomPos,
					&bottomCenter->getSourceRect(), tint, rotation, origin, scale, SpriteEffects_None, layerDepth);

				width = topCenter->getWidth();
				topPos.x += width;
				bottomPos.x += width;
			}

			// draw bottom corners
			batch->Draw(topRightCorner->getTexture().Get(), topPos,
				&topRightCorner->getSourceRect(), tint, rotation, origin, scale, SpriteEffects_None, layerDepth);

			batch->Draw(bottomRightCorner->getTexture().Get(), bottomPos,
				&bottomRightCorner->getSourceRect(), tint, rotation, origin, scale, SpriteEffects_None, layerDepth);


			// draw verticals
			Vector2 leftPos = position;
			Vector2 rightPos = position;
			//rightPos.x += size.x - topLeftCorner->getWidth();
			rightPos.x = bottomPos.x;

			int height = topLeftCorner->getHeight();
			maxHeight = position.y + size.y - height;
			leftPos.y += height;
			rightPos.y += height;
			while (leftPos.y < maxHeight) {

				batch->Draw(centerLeft->getTexture().Get(), leftPos,
					&centerLeft->getSourceRect(), tint, rotation, origin, scale, SpriteEffects_None, layerDepth);

				batch->Draw(centerRight->getTexture().Get(), rightPos,
					&centerRight->getSourceRect(), tint, rotation, origin, scale, SpriteEffects_None, layerDepth);

				height = centerLeft->getHeight();
				leftPos.y += height;
				rightPos.y += height;
			}
		}
	}
	dialogText->draw(batch);
}

void DynamicDialog::setPosition(const Vector2& newPosition) {

	
	Vector2 moveBy = newPosition - position;
	dialogText->moveBy(moveBy);
	GUIControl::setPosition(newPosition);
	panel->setPosition(newPosition);
}

void DynamicDialog::setLayerDepth(const float depth, bool frontToBack) {

	layerDepth = depth - .00001;
	if (layerDepth < 0) {
		if (!frontToBack)
			layerDepth = .00001;
		else
			layerDepth = 0;
	}
	float nudge = .00000001;
	if (!frontToBack)
		nudge *= -1;
	dialogText->setLayerDepth(layerDepth + nudge, frontToBack);
		/*for (const auto& sprite : bgSprites)
			sprite->setLayerDepth(depth, frontToBack);*/
}

void DynamicDialog::show() {
	Dialog::show();
}

void DynamicDialog::close() {
	Dialog::close();

	OutputDebugString(L"Dialog closing\n");
}

bool DynamicDialog::isShowing() {
	return isOpen;
}

