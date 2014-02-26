//------------------------------------------------------------------------------------------
// File: TextureManager.cpp
//
// This class is used to avoid loading duplicate textures
//------------------------------------------------------------------------------------------

#include "TextureManager.h"

// Must be included last!
#include "shared/debug.h"

TextureManager::TextureManager(void)
	: md3dDevice(0)
{
}


TextureManager::~TextureManager(void)
{
	for(auto it = mLevelTextureSRV.begin(); it != mLevelTextureSRV.end(); ++it)
	{
		//ReleaseCOM(it->second);
		//it->second = nullptr;
	}

	mLevelTextureSRV.clear();

	for (auto it = mStaticTextureSRV.begin(); it != mStaticTextureSRV.end(); ++it)
	{
		ReleaseCOM(it->second);
	}

	mStaticTextureSRV.clear();
}

void TextureManager::Init(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	md3dDevice = device;
	mDC = dc;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureManager::CreateLevelTexture(std::string fileName)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;
	//std::shared_ptr<ID3D11ShaderResourceView> srv = nullptr;
	//ID3D11ShaderResourceView* srv = 0;

	// Find if texture already exists
	if (mLevelTextureSRV.find(fileName) != mLevelTextureSRV.end())
	{
		//srv = mLevelTextureSRV[fileName]; // Just point to existing texture
		srv = mLevelTextureSRV[fileName].Get(); // Just point to existing texture
	}

	// Otherwise create the new texture
	else
	{
		std::wstring path(fileName.begin(), fileName.end());

		//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, path.c_str(), 0, 0, &srv, 0));

		HRESULT hr;

		// Try loading the texture as dds
		hr = CreateDDSTextureFromFile(md3dDevice.Get(), path.c_str(), nullptr, &srv);

		// Failed loading texture (not .dds) - assume it's another format
		if (srv == NULL)
			hr = CreateWICTextureFromFile(md3dDevice.Get(), mDC.Get(), path.c_str(), nullptr, &srv);

		// Texture loading still failed, format either unsupported or file doesn't exist
		if (srv == NULL)
		{
			std::wostringstream ErrorStream;
			ErrorStream << "Failed to load texture " << path;
			MessageBox(0, ErrorStream.str().c_str(), 0, 0);
		}

		mLevelTextureSRV[fileName] = srv.Get();
	}

	//return srv;
	return srv;
}

/*
ID3D11ShaderResourceView* TextureManager::CreateLevelTexture(std::string fileName)
{
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;
	//std::shared_ptr<ID3D11ShaderResourceView> srv = nullptr;
	ID3D11ShaderResourceView* srv = 0;

	// Find if texture already exists
	if (mLevelTextureSRV.find(fileName) != mLevelTextureSRV.end())
	{
		//srv = mLevelTextureSRV[fileName]; // Just point to existing texture
		srv = mLevelTextureSRV[fileName].get(); // Just point to existing texture
	}

	// Otherwise create the new texture
	else
	{
		std::wstring path(fileName.begin(), fileName.end());

		//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, path.c_str(), 0, 0, &srv, 0));

		HRESULT hr;

		// Try loading the texture as dds
		hr = CreateDDSTextureFromFile(md3dDevice.Get(), path.c_str(), nullptr, &srv);

		// Failed loading texture (not .dds) - assume it's another format
		if (srv == NULL)
			hr = CreateWICTextureFromFile(md3dDevice.Get(), mDC.Get(), path.c_str(), nullptr, &srv);

		// Texture loading still failed, format either unsupported or file doesn't exist
		if (srv == NULL)
		{
			std::wostringstream ErrorStream;
			ErrorStream << "Failed to load texture " << path;
			MessageBox(0, ErrorStream.str().c_str(), 0, 0);
		}

		mLevelTextureSRV[fileName] = srv;
	}

	//return srv;
	return srv;
}
*/

/*
ID3D11ShaderResourceView* TextureManager::CreateDDSTextureFromBytes(const unsigned char* bytes, UINT byteSize, std::string textureName)
{
	ID3D11ShaderResourceView* srv = 0;

	// Find if texture already exists
	if (mLevelTextureSRV.find(textureName) != mLevelTextureSRV.end())
	{
		//srv = mLevelTextureSRV[textureName]; // Just point to existing texture
		srv = mLevelTextureSRV[textureName].get();
	}

	HRESULT hr;
	hr = CreateDDSTextureFromMemory(md3dDevice.Get(), bytes, byteSize, nullptr, &srv);

	if (FAILED(hr))
	{
		std::wostringstream ErrorStream;
		ErrorStream << "Failed to create DDS texture " << textureName.c_str() << " from memory!";
		MessageBox(0, ErrorStream.str().c_str(), 0, 0);
	}

	mLevelTextureSRV[textureName] = srv;

	return srv;
}
*/

void TextureManager::ClearLevel()
{
	for (auto it = mLevelTextureSRV.begin(); it != mLevelTextureSRV.end(); ++it)
	{
// 		if (it->second.unique())
// 		{
// 			ReleaseCOM(it->second);
// 		}
	}

	//mLevelTextureSRV.clear();
}

void TextureManager::DeleteLevelTexture(std::string fileName)
{
	/*
	if (mLevelTextureSRV.find(fileName) != mLevelTextureSRV.end())
	{
		ReleaseCOM(mLevelTextureSRV[fileName]);
		mLevelTextureSRV.erase(fileName);
	}
	*/
}

void TextureManager::DeleteLevelTexture(ID3D11ShaderResourceView* srv)
{
	for (auto it = mLevelTextureSRV.begin(); it != mLevelTextureSRV.end();)
	{
		if (it->second.Get() == srv)
		{
			it->second = nullptr;
			mLevelTextureSRV.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

ID3D11ShaderResourceView* TextureManager::CreateStaticTexture(std::string fileName)
{
	ID3D11ShaderResourceView* srv = 0;

	// Find if texture already exists
	if (mStaticTextureSRV.find(fileName) != mStaticTextureSRV.end())
	{
		srv = mStaticTextureSRV[fileName]; // Just point to existing texture
	}

	// Otherwise create the new texture
	else
	{
		std::wstring path(fileName.begin(), fileName.end());

		//HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, path.c_str(), 0, 0, &srv, 0));

		HRESULT hr;

		// Try loading the texture as dds
		hr = CreateDDSTextureFromFile(md3dDevice.Get(), path.c_str(), nullptr, &srv);

		// Failed loading texture (not .dds) - assume it's another format
		if (srv == NULL)
			hr = CreateWICTextureFromFile(md3dDevice.Get(), mDC.Get(), path.c_str(), nullptr, &srv);

		// Texture loading still failed, format either unsupported or file doesn't exist
		if (srv == NULL)
		{
			std::wostringstream ErrorStream;
			ErrorStream << "Failed to load texture " << path;
			MessageBox(0, ErrorStream.str().c_str(), 0, 0);
		}

		mStaticTextureSRV[fileName] = srv;
	}

	return srv;
}
