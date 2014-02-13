#ifndef FULLSCREENTRIANGLE_H_
#define FULLSCREENTRIANGLE_H_

#include <d3d11.h>
#include "Vertex.h"
#include <DirectXMath.h>

using namespace DirectX;

class FullscreenTriangle
{
public:
	FullscreenTriangle(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	~FullscreenTriangle();

	void Draw(ID3D11DeviceContext* dc);

private:
	ID3D11Buffer* mBuffer;
	ID3D11InputLayout* mInputLayout;
};

#endif