#include <cassert>

#include "Texture2DImpl.h"

Texture2DImpl::Texture2DImpl(ID3D11Device *d3dDevice, ID3D11DeviceContext *d3dDeviceContext, unsigned int width, unsigned int height, DXGI_FORMAT format, UINT bindFlags, D3D11_USAGE usage, UINT CPUAccessFlags)
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
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = usage;
	texDesc.BindFlags = bindFlags;
	texDesc.CPUAccessFlags = CPUAccessFlags;
	texDesc.MiscFlags = 0;

	HRESULT hr = d3dDevice->CreateTexture2D(&texDesc, NULL, &m_texture);

	assert(SUCCEEDED(hr));
	assert(m_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(SRVDesc));
	SRVDesc.Format = format;
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

unsigned int Texture2DImpl::GetWidth() const
{
	return m_width;
}

unsigned int Texture2DImpl::GetHeight() const
{
	return m_height;
}

void Texture2DImpl::UploadData(const void *data)
{
	m_d3dDeviceContext->UpdateSubresource(m_texture, 0, NULL, data, 4 * m_width, 0);
}

ID3D11Texture2D *Texture2DImpl::GetTexture()
{
	return m_texture;
}

ID3D11ShaderResourceView *Texture2DImpl::GetTextureView()
{
	return m_textureView;
}
