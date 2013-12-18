//------------------------------------------------------------------------------------------
// File: TextureManager.cpp
//
// This class is used to avoid loading duplicate textures
//------------------------------------------------------------------------------------------

#include "TextureManager.h"

TextureManager::TextureManager(void)
	: md3dDevice(0)
{
}


TextureManager::~TextureManager(void)
{
	for(auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
	{
		ReleaseCOM(it->second);
	}

	mTextureSRV.clear();
}

void TextureManager::Init(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	md3dDevice = device;
	mDC = dc;
}

ID3D11ShaderResourceView* TextureManager::CreateTexture(std::string fileName)
{
	ID3D11ShaderResourceView* srv = 0;

	// Find if texture already exists
	if (mTextureSRV.find(fileName) != mTextureSRV.end())
	{
		srv = mTextureSRV[fileName]; // Just point to existing texture
	}

	// Otherwise create the new texture
	else
	{
		std::wstring path(fileName.begin(), fileName.end());

		//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, path.c_str(), 0, 0, &srv, 0));

		HRESULT hr;

		// Try loading the texture as dds
		hr = CreateDDSTextureFromFile(md3dDevice, path.c_str(), nullptr, &srv);

		// Failed loading texture (not .dds) - assume it's another format
		if (srv == NULL)
			hr = CreateWICTextureFromFile(md3dDevice, mDC, path.c_str(), nullptr, &srv);

		// Texture loading still failed, format either unsupported or file doesn't exist
		if (srv == NULL)
		{
			std::wostringstream ErrorStream;
			ErrorStream << "Failed to load texture " << path;
			MessageBox(0, ErrorStream.str().c_str(), 0, 0);
		}

		mTextureSRV[fileName] = srv;
	}

	return srv;
}
