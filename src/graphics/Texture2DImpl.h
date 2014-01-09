#ifndef GRAPHICS_TEXTURE2DIMPL_H
#define GRAPHICS_TEXTURE2DIMPL_H

#include <d3d11.h>

#include "Texture2D.h"

class Texture2DImpl : public Texture2D
{
public:
	Texture2DImpl(ID3D11Device *d3dDevice, ID3D11DeviceContext *d3dDeviceContext, unsigned int width, unsigned int height, DXGI_FORMAT format, UINT bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT CPUAccessFlags = 0);
	~Texture2DImpl();

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	void UploadData(const void *data);

	ID3D11Texture2D *GetTexture();
	ID3D11ShaderResourceView *GetTextureView();

private:
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext *m_d3dDeviceContext;
	ID3D11Texture2D *m_texture;
	ID3D11ShaderResourceView *m_textureView;

	unsigned int m_width;
	unsigned int m_height;
};

#endif
