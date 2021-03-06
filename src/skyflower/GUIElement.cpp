#include "GUIElement.h"

// Must be included last!
#include "shared/debug.h"

GUIElement::GUIElement()
{
	this->mTexture = nullptr;
}

GUIElement::GUIElement(const GUIElement& other)
{
	this->mDrawInput = other.mDrawInput;
	this->mTexture = other.mTexture;
}

GUIElement::~GUIElement()
{

}

void GUIElement::Destroy(GraphicsEngine* gEngine)
{
	gEngine->DeleteTexture2D(this->mTexture);
}

void GUIElement::Draw(GraphicsEngine* gEngine)
{
	if (this->mIsVisible)
	{
		if (this->mFromFile)
			gEngine->Draw2DTextureFile(this->mFile, &this->mDrawInput);
		else
			gEngine->Draw2DTexture(this->mTexture, &this->mDrawInput);
	}
}

void GUIElement::UploadTextureData(const void* data)
{
	this->mTexture->UploadData(data);
}

/* Text Element */

TextElement::TextElement()
{
	this->color = Vec3::Zero();
	this->x = 0;
	this->y = 0;
	this->text = nullptr;
	this->scale = 0.0f;
}

TextElement::TextElement(std::string text, int x, int y, Vec3 color, float alpha, float scale)
{
	this->color = color;
	this->x = x;
	this->y = y;
	this->text = text;
	this->scale = scale;
	this->alpha = alpha;
}
void TextElement::Draw(GraphicsEngine* gEngine)
{
	gEngine->PrintText(text, x, y, color, alpha, scale);
}