#ifndef GRAPHICS_TEXTURE2DIMPL_H
#define GRAPHICS_TEXTURE2DIMPL_H

#include <d3d11.h>
#include "d3dUtilities.h"

#include "Texture2D.h"

class Texture2DImpl : public Texture2D
{
public:
	Texture2DImpl(ID3D11Device *d3dDevice, ID3D11DeviceContext *d3dDeviceContext, unsigned int width, unsigned int height, DXGI_FORMAT format, bool renderable);
	~Texture2DImpl();

	void UploadData(const void *data);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	bool IsRenderable() const;

	ID3D11Texture2D *GetTexture() const;
	ID3D11ShaderResourceView *GetShaderResourceView() const;
	ID3D11RenderTargetView *GetRenderTargetView() const;

	void Resize(ID3D11Device* d3dDevice, int width, int height);

private:
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext *m_d3dDeviceContext;

	ID3D11Texture2D *m_texture;
	ID3D11ShaderResourceView *m_shaderResourceView;
	ID3D11RenderTargetView *m_renderTargetView;

	unsigned int m_width;
	unsigned int m_height;
	bool mRenderable;
	DXGI_FORMAT mFormat;
};

#endif
