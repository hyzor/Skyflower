//------------------------------------------------------------------------------------------
// File: TextureManager.h
//
// This class is used to avoid loading duplicate textures
//------------------------------------------------------------------------------------------

#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include "d3dUtilities.h"
#include <map>
#include <DDSTextureLoader.h>

class TextureManager
{
public:
	TextureManager(void);
	~TextureManager(void);

	void Init(ID3D11Device* device);

	ID3D11ShaderResourceView* CreateTexture(std::string fileName);

private:
	ID3D11Device* md3dDevice;
	std::map<std::string, ID3D11ShaderResourceView*> mTextureSRV;
};

#endif
