#include <cassert>

#include "Texture2DImpl.h"

// Must be included last!
#include "shared/debug.h"

Texture2DImpl::Texture2DImpl(ID3D11Device *d3dDevice, ID3D11DeviceContext *d3dDeviceContext, unsigned int width, unsigned int height, DXGI_FORMAT format, bool renderable)
{
	m_d3dDevice = d3dDevice;
	m_d3dDeviceContext = d3dDeviceContext;

	m_texture = NULL;
	m_shaderResourceView = NULL;
	m_renderTargetView = NULL;

	m_width = width;
	m_height = height;

	mRenderable = renderable;
	mFormat = format;

	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (renderable) {
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = bindFlags;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = d3dDevice->CreateTexture2D(&texDesc, NULL, &m_texture);

	assert(SUCCEEDED(hr));
	assert(m_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	
	hr = d3dDevice->CreateShaderResourceView(m_texture, &SRVDesc, &m_shaderResourceView);

	assert(SUCCEEDED(hr));
	assert(m_shaderResourceView);

	if (renderable) {
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		memset(&renderTargetViewDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		renderTargetViewDesc.Format = format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		hr = d3dDevice->CreateRenderTargetView(m_texture, &renderTargetViewDesc, &m_renderTargetView);

		assert(SUCCEEDED(hr));
		assert(m_renderTargetView);
	}
}

Texture2DImpl::~Texture2DImpl(void)
{
// 	if (m_renderTargetView) {
// 		m_renderTargetView->Release();
// 	}

	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_shaderResourceView);
	ReleaseCOM(m_texture);

// 	m_shaderResourceView->Release();
// 	m_texture->Release();
}

void Texture2DImpl::UploadData(const void *data)
{
	m_d3dDeviceContext->UpdateSubresource(m_texture, 0, NULL, data, 4 * m_width, 0);
}

unsigned int Texture2DImpl::GetWidth() const
{
	return m_width;
}

unsigned int Texture2DImpl::GetHeight() const
{
	return m_height;
}

ID3D11Texture2D *Texture2DImpl::GetTexture() const
{
	return m_texture;
}

ID3D11ShaderResourceView *Texture2DImpl::GetShaderResourceView() const
{
	return m_shaderResourceView;
}

ID3D11RenderTargetView *Texture2DImpl::GetRenderTargetView() const
{
	return m_renderTargetView;
}

void Texture2DImpl::Resize(ID3D11Device* d3dDevice, int width, int height)
{
	if (m_renderTargetView) {
		m_renderTargetView->Release();
	}

	m_shaderResourceView->Release();
	m_texture->Release();

	m_width = width;
	m_height = height;

	UINT bindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (mRenderable) {
		bindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = mFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = bindFlags;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = d3dDevice->CreateTexture2D(&texDesc, NULL, &m_texture);

	assert(SUCCEEDED(hr));
	assert(m_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = mFormat;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = d3dDevice->CreateShaderResourceView(m_texture, &SRVDesc, &m_shaderResourceView);

	assert(SUCCEEDED(hr));
	assert(m_shaderResourceView);

	if (mRenderable) 
	{
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		memset(&renderTargetViewDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		renderTargetViewDesc.Format = mFormat;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		hr = d3dDevice->CreateRenderTargetView(m_texture, &renderTargetViewDesc, &m_renderTargetView);

		assert(SUCCEEDED(hr));
		assert(m_renderTargetView);
	}
}
