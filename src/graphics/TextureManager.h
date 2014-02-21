//------------------------------------------------------------------------------------------
// File: TextureManager.h
//
// This class is used to avoid loading duplicate textures
//------------------------------------------------------------------------------------------

#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include "d3dUtilities.h"
#include <map>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>

class TextureManager
{
public:
	TextureManager(void);
	~TextureManager(void);

	void Init(ID3D11Device* device, ID3D11DeviceContext* dc);

	ID3D11ShaderResourceView* CreateTexture(std::string fileName);

	ID3D11ShaderResourceView* CreateDDSTextureFromBytes(const unsigned char* bytes, UINT byteSize, std::string textureName);

private:
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* mDC;
	std::map<std::string, ID3D11ShaderResourceView*> mTextureSRV;
};

#endif
