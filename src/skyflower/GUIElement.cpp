#include "GUIElement.h"

GUIElement::GUIElement()
{
	this->mDrawInput = new Draw2DInput();
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
			gEngine->Draw2DTextureFile(this->mFile, this->mDrawInput);
		else
			gEngine->Draw2DTexture(this->mTexture, this->mDrawInput);
	}
}

void GUIElement::UploadTextureData(const void* data)
{
	this->mTexture->UploadData(data);
}