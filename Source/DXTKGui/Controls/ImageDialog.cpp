#include "ImageDialog.h"

ImageDialog::ImageDialog(bool canMove, bool centerTxt) {

	movable = canMove;
	centerText = centerTxt;
}

ImageDialog::~ImageDialog() {
	bgSprites.clear();
}

#include "GUIFactory.h"
void ImageDialog::initializeControl(GUIFactory * factory,
	shared_ptr<MouseController> mouseController) {
	GUIControl::initializeControl(factory, mouseController);
}


void ImageDialog::initialize(shared_ptr<AssetSet> set, const pugi::char_t* font) {

	assetSet = set;

	dialogText = make_unique<TextLabel>(guiFactory->getFont(font));
	dialogText->setTint(Color(0, 0, 0, 1));

	setLayerDepth(.95);

	dialogText = make_unique<TextLabel>(guiFactory->getFont(font));
	dialogText->setTint(Color(0, 0, 0, 1));
	dialogText->setPosition(Vector2::Zero);
}


void ImageDialog::setText(wstring text) {
	dialogText->setText(text);
}

void ImageDialog::setDimensions(const Vector2& position, const Vector2& size, const int frameThickness) {
	//for (const pugi::char_t* asset : assets) {

	this->position = position;
	Vector2 pos = position;

	unique_ptr<Sprite> sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Top Left Corner"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x += sprite->getWidth();
	bgSprites.push_back(move(sprite));

	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Top Center"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x += sprite->getWidth();
	bgSprites.push_back(move(sprite));

	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Top Center"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x += sprite->getWidth();
	bgSprites.push_back(move(sprite));

	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Top Right Corner"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x = position.x;
	pos.y += sprite->getHeight();
	bgSprites.push_back(move(sprite));

	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Center Left"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x += sprite->getWidth();
	bgSprites.push_back(move(sprite));


	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Middle"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x += sprite->getWidth();
	bgSprites.push_back(move(sprite));


	dialogText->setPosition(position);


	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Center Right"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x = position.x;
	pos.y += sprite->getHeight() * 2;
	bgSprites.push_back(move(sprite));




	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Bottom Left Corner"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x += sprite->getWidth();
	bgSprites.push_back(move(sprite));

	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Bottom Center"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x += sprite->getWidth();
	bgSprites.push_back(move(sprite));

	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Bottom Center"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	pos.x += sprite->getWidth();
	bgSprites.push_back(move(sprite));



	sprite = make_unique<Sprite>();
	sprite->load(assetSet->getAsset("Bottom Right Corner"));
	sprite->setPosition(pos);
	sprite->setOrigin(Vector2::Zero);
	bgSprites.push_back(move(sprite));


}

void ImageDialog::draw(SpriteBatch* batch) {

	for (const auto& sprite : bgSprites) {
		sprite->draw(batch);
		dialogText->draw(batch);

	}
}

void ImageDialog::setLayerDepth(const float depth, bool frontToBack) {

	for (const auto& sprite : bgSprites)
		sprite->setLayerDepth(depth, frontToBack);
}
