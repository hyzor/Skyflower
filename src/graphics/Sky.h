//------------------------------------------------------------------------------------
// File: Sky.h
//
// This class renders a sky using a cube map
//------------------------------------------------------------------------------------

#ifndef SKY_H_
#define SKY_H_

#include "d3dUtilities.h"
#include "Camera.h"
#include <DDSTextureLoader.h>
#include "ShaderHandler.h"
#include "RenderStates.h"
#include "TextureManager.h"

class Sky
{
public:
	Sky(ID3D11Device* device, TextureManager *textureManager, const std::string& cubeMapFileName, float skySphereRadius);
	~Sky(void);

	ID3D11ShaderResourceView* cubeMapSRV();

	void Draw(ID3D11DeviceContext* dc, const Camera& cam, SkyShader* skyShader);
	void Draw(ID3D11DeviceContext* dc, const Camera& cam, SkyDeferredShader* skyShader);

private:
	// Buffers
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	// Texture
	ID3D11ShaderResourceView* mCubeMapSRV;

	UINT mIndexCount;
};

#endif