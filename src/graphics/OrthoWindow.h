#ifndef ORTHOWINDOW_H_
#define ORTHOWINDOW_H_

#include <d3d11.h>
#include "Vertex.h"

class OrthoWindow
{
public:
	OrthoWindow();
	~OrthoWindow();

	bool Initialize(ID3D11Device* device, int windowWidth, int windowHeight);
	void Shutdown();
	void Render(ID3D11DeviceContext* dc);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device, int windowWidth, int windowHeight);
	void ShutdownBuffers();
	void PrepareBuffers(ID3D11DeviceContext* dc);

private:
	ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
	int mVertexCount, mIndexCount;
};

#endif