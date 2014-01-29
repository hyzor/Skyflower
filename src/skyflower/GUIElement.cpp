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