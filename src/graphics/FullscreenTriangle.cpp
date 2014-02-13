#include "FullscreenTriangle.h"


FullscreenTriangle::FullscreenTriangle(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA data;
	Vertex::PosTex vertices[3];

	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	vertices[1].position = XMFLOAT3(-1.0f, 3.0f, 1.0f);
	vertices[2].position = XMFLOAT3(3.0f, -1.0f, 1.0f);

	vertices[0].texCoord = XMFLOAT2(0.0f, 1.0f);
	vertices[1].texCoord = XMFLOAT2(0.0f, -1.0f);
	vertices[2].texCoord = XMFLOAT2(2.0f, 1.0f);

	bufferDesc.ByteWidth = sizeof(Vertex::PosTex) * 3;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	data.pSysMem = vertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = device->CreateBuffer(&bufferDesc, &data, &mBuffer);

	mInputLayout = inputLayout;
}


FullscreenTriangle::~FullscreenTriangle()
{
	if (mBuffer)
		mBuffer->Release();
}

void FullscreenTriangle::Draw(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(mInputLayout);

	const UINT offset = 0;
	const UINT stride = sizeof(Vertex::PosTex);
	dc->IASetVertexBuffers(0, 1, &mBuffer, &stride, &offset);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->Draw(3, 0);
}
