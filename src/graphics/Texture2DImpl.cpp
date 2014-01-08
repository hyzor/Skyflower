#include <cassert>

#include "Texture2DImpl.h"

Texture2DImpl::Texture2DImpl(ID3D11Device *d3dDevice, ID3D11DeviceContext *d3dDeviceContext, unsigned int width, unsigned int height)
{
	m_d3dDevice = d3dDevice;
	m_d3dDeviceContext = d3dDeviceContext;
	m_texture = NULL;
	m_textureView = NULL;
	m_width = width;
	m_height = height;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = d3dDevice->CreateTexture2D(&texDesc, NULL, &m_texture);

	assert(SUCCEEDED(hr));
	assert(m_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	
	hr = d3dDevice->CreateShaderResourceView(m_texture, &SRVDesc, &m_textureView);

	assert(SUCCEEDED(hr));
	assert(m_textureView);
}

Texture2DImpl::~Texture2DImpl(void)
{
	m_texture->Release();
	m_textureView->Release();
}

void Texture2DImpl::UploadData(const void *data)
{
	m_d3dDeviceContext->UpdateSubresource(m_texture, 0, NULL, data, 4 * m_width, 0);
}

ID3D11ShaderResourceView *Texture2DImpl::GetTextureView()
{
	return m_textureView;
}
