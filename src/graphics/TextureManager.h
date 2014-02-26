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

	void ClearLevel();

	//ID3D11ShaderResourceView* CreateLevelTexture(std::string fileName);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateLevelTexture(std::string fileName);
	void DeleteLevelTexture(std::string fileName);
	void DeleteLevelTexture(ID3D11ShaderResourceView* srv);

	ID3D11ShaderResourceView* CreateStaticTexture(std::string fileName);

	//ID3D11ShaderResourceView* CreateDDSTextureFromBytes(const unsigned char* bytes, UINT byteSize, std::string textureName);

	std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> mLevelTextureSRV;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDC;

	//ID3D11Device* md3dDevice;
	//ID3D11DeviceContext* mDC;
	//std::map<std::string, ID3D11ShaderResourceView*> mLevelTextureSRV;
	//std::map<std::string, std::shared_ptr<ID3D11ShaderResourceView>> mLevelTextureSRV;
	std::map<std::string, ID3D11ShaderResourceView*> mStaticTextureSRV;
};

#endif
