#include "GUI.h"
#include "Graphics/Texture2DImpl.h"

// Must be included last!
#include "shared/debug.h"

GUI::GUI()
{
	this->mCurrGUIElementId = 0;
}

GUI::GUI(const GUI& other)
{

}

GUI::~GUI()
{

}

void GUI::Destroy(GraphicsEngine* gEngine)
{
	for (unsigned int i = 0; i < this->mGUIElements.size(); i++)
	{
		this->mGUIElements.at(i)->Destroy(gEngine);
	}
}

void GUI::Draw(GraphicsEngine* gEngine)
{
	//TO ADD: Send parameters along with the Begin2D call (using the drawing options for the GUI that are to be added)
	gEngine->Begin2D();
	for (unsigned int i = 0; i < this->mGUIElements.size(); i++)
	{
		this->mGUIElements.at(i)->Draw(gEngine);
	}
	gEngine->End2D();
}


Texture2D* GUI::CreateTexture2D(GraphicsEngine* gEngine, unsigned int width, unsigned int height)
{
	return gEngine->CreateTexture2D(width, height);
}

void GUI::CreateTextObject()
{

}

int GUI::CreateGUIElement(Vec3 pos)
{
	GUIElement* elem = new GUIElement();

	//Set drawing specific values
	Draw2DInput* input = elem->GetDrawInput();
	input->pos = XMFLOAT2(pos.X, pos.Y);
	input->origin = XMFLOAT2(0.0f, 0.0f);
	input->scale = XMFLOAT2(1.0f, 1.0f); 
	input->color = XMLoadFloat4(&XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	input->rot = 0.0f;
	input->layerDepth = 0.0f;

	//Set values used for managing
	elem->SetID(this->mCurrGUIElementId);
	elem->SetVisible(true);

	this->mCurrGUIElementId++;
	this->mGUIElements.push_back(elem);

	return mCurrGUIElementId - 1;
}

void GUI::BindTextureToGUIElement(unsigned int id, Texture2D* texture, std::string file)
{
	GUIElement* elem =  this->_GetGUIElement(id);

	elem->SetFile(STANDARD_TEXTURE_PATH + file);
	elem->SetTexture(texture);
}

int GUI::CreateGUIElementAndBindTexture(Vec3 pos, std::string file)
{
	GUIElement* elem = new GUIElement();

	//Set drawing specific values
	Draw2DInput* input = elem->GetDrawInput();
	input->pos = XMFLOAT2(pos.X, pos.Y);
	input->origin = XMFLOAT2(0.0f, 0.0f);
	input->scale = XMFLOAT2(1.0f, 1.0f);
	input->color = XMLoadFloat4(&XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	input->rot = 0.0f;
	input->layerDepth = 0.0f;

	//Set values used for managing
	elem->SetID(this->mCurrGUIElementId);
	elem->SetVisible(true);
	elem->SetFromFile(true);
	elem->SetFile(STANDARD_TEXTURE_PATH + file);

	this->mCurrGUIElementId++;
	this->mGUIElements.push_back(elem);

	return mCurrGUIElementId - 1;
}

int GUI::CreateGUIElementAndBindTexture(Vec3 pos, Texture2D* texture)
{
	GUIElement* elem = new GUIElement();

	elem->SetTexture(texture);

	//Set drawing specific values
	Draw2DInput* input = elem->GetDrawInput();
	input->pos = XMFLOAT2(pos.X, pos.Y);
	input->origin = XMFLOAT2(0.0f, 0.0f);
	input->scale = XMFLOAT2(1.0f, 1.0f);
	input->color = XMLoadFloat4(&XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	input->rot = 0.0f;
	input->layerDepth = 0.0f;

	//Set values used for managing
	elem->SetID(this->mCurrGUIElementId);
	elem->SetVisible(true);
	elem->SetFromFile(false);

	this->mCurrGUIElementId++;
	this->mGUIElements.push_back(elem);

	return mCurrGUIElementId - 1;
}

GUIElement* GUI::_GetGUIElement(unsigned int id)
{
	GUIElement* elem = nullptr;

	for (unsigned int i = 0; i < this->mGUIElements.size(); i++)
	{
		if (this->mGUIElements.at(i)->GetID() == id)
		{
			elem = this->mGUIElements.at(i);
		}
	}

	return elem;
}

GUIElement* GUI::GetGUIElement(unsigned int id)
{
	return _GetGUIElement(id);
}

void GUI::HideGUI()
{
	for (unsigned int i = 0; i < this->mGUIElements.size(); i++)
	{
		this->mGUIElements.at(i)->SetVisible(false);
	}
}

void GUI::UploadData(unsigned int id, const void* data)
{
	_GetGUIElement(id)->UploadTextureData(data);
}
