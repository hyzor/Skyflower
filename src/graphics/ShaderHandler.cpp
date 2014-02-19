#include "ShaderHandler.h"

// Must be included last!
//#include "shared/debug.h"

#pragma region ShaderHandler
ShaderHandler::ShaderHandler()
{
	mBasicShader = new BasicShader();
	mSkyShader = new SkyShader();
	mNormalSkinned = new NormalMappedSkinned(); 
	mBasicDeferredShader = new BasicDeferredShader();
	mBasicDeferredSkinnedShader = new BasicDeferredSkinnedShader();
	mLightDeferredShader = new LightDeferredShader();
	mShadowShader = new ShadowShader();
	mSkinnedShadowShader = new SkinnedShadowShader();
	mSkyDeferredShader = new SkyDeferredShader();
	mSSAOShader = new SSAOShader();
	mSSAOBlurHorizontalShader = new BlurShader();
	mSSAOBlurVerticalShader = new BlurShader();
	mDepthOfFieldCoCShader = new DepthOfFieldCoCShader();
	mDepthOfFieldBlurHorizontalShader = new BlurShader();
	mDepthOfFieldBlurVerticalShader = new BlurShader();
	mCompositeShader = new CompositeShader();
	mDeferredMorphShader = new BasicDeferredMorphShader();
	mShadowMorphShader = new ShadowMorphShader();
	mParticleSystemShader = new ParticleSystemShader();
	mLightDeferredToTextureShader = new LightDeferredShader();
	mBasicDeferredSkinnedSortedShader = new BasicDeferredSkinnedSortedShader();

	// SMAA
	mSMAAColorEdgeDetectionShader = new SMAAColorEdgeDetectionShader();
	mSMAADepthEdgeDetectionShader = new SMAADepthEdgeDetectionShader();
	mSMAALumaEdgeDetectionShader = new SMAALumaEdgeDetectionShader();
	mSMAABlendingWeightCalculationsShader = new SMAABlendingWeightCalculationsShader();
	mSMAANeighborhoodBlendingShader = new SMAANeighborhoodBlendingShader();
}

ShaderHandler::~ShaderHandler()
{
	for (UINT i = 0; i < mShaders.size(); ++i)
	{
		mShaders[i]->Buffer->Release();
		delete mShaders[i];
	}

	for (auto& it(mVertexShaders.begin()); it != mVertexShaders.end(); ++it)
	{
		if (it->second)
			it->second->Release();
	}
	mVertexShaders.clear();

	for (auto& it(mPixelShaders.begin()); it != mPixelShaders.end(); ++it)
	{
		if (it->second)
			it->second->Release();
	}
	mPixelShaders.clear();

	for (auto& it(mGeometryShaders.begin()); it != mGeometryShaders.end(); ++it)
	{
		if (it->second)
			it->second->Release();
	}
	mGeometryShaders.clear();

	delete mBasicShader;
	delete mSkyShader;
	delete mNormalSkinned;
	delete mShadowShader;
	delete mBasicDeferredShader;
	delete mBasicDeferredSkinnedShader;
	delete mLightDeferredShader;
	delete mSkinnedShadowShader;
	delete mSkyDeferredShader;
	delete mSSAOShader;
	delete mSSAOBlurHorizontalShader;
	delete mSSAOBlurVerticalShader;
	delete mDepthOfFieldCoCShader;
	delete mDepthOfFieldBlurHorizontalShader;
	delete mDepthOfFieldBlurVerticalShader;
	delete mCompositeShader;
	delete mDeferredMorphShader;
	delete mShadowMorphShader;
	delete mParticleSystemShader;
	delete mLightDeferredToTextureShader;

	delete mBasicDeferredSkinnedSortedShader;

	// SMAA
	delete mSMAAColorEdgeDetectionShader;
	delete mSMAADepthEdgeDetectionShader;
	delete mSMAALumaEdgeDetectionShader;
	delete mSMAABlendingWeightCalculationsShader;
	delete mSMAANeighborhoodBlendingShader;
}

void ShaderHandler::LoadCompiledVertexShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = VERTEXSHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load vertex shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11VertexShader* vShader;
	hr = device->CreateVertexShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &vShader);
	mVertexShaders[name] = vShader;
}

void ShaderHandler::LoadCompiledPixelShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = PIXELSHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load pixel shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11PixelShader* pShader;
	hr = device->CreatePixelShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &pShader);
	mPixelShaders[name] = pShader;
}

void ShaderHandler::LoadCompiledGeometryShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = GEOMETRYSHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load geometry shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11GeometryShader* gShader;
	hr = device->CreateGeometryShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &gShader);
	mGeometryShaders[name] = gShader;
}

void ShaderHandler::LoadCompiledGeometryStreamOutShader(LPCWSTR fileName, char* name, ID3D11Device* device, D3D11_SO_DECLARATION_ENTRY pDecl[], UINT pDeclSize, UINT rasterizedStream, UINT* bufferStrides, UINT numStrides)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = GEOMETRYSHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load geometry stream out shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11GeometryShader* gShader;
// 	hr = device->CreateGeometryShader(mShaders.back()->Buffer->GetBufferPointer(),
// 		mShaders.back()->Buffer->GetBufferSize(), nullptr, &gShader);

// 	mGeometryShaders[name] = gShader;

	hr = device->CreateGeometryShaderWithStreamOutput(
		mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(),
		/*Stream out declaration*/pDecl,
		pDeclSize,
		bufferStrides, // sizeof(Vertex::Particle)
		numStrides, // One stride
		rasterizedStream,
		NULL,
		&gShader);

	mGeometryShaders[name] = gShader;
}

BYTE* ShaderHandler::LoadByteCode(char* fileName, UINT& size)
{
	FILE* file = 0;
	unsigned char* shaderCode = 0;
	UINT fileSize = 0;
	UINT numRead = 0;

	//file = fopen(fileName, "rb");

	fopen_s(&file, fileName, "rb");

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	shaderCode = (unsigned char*)malloc(fileSize);

	while (numRead != fileSize)
		numRead = (UINT)fread(&shaderCode[numRead], 1, fileSize, file);

	fclose(file);

	size = fileSize;

	return shaderCode;
}

ID3D11VertexShader* ShaderHandler::GetVertexShader(std::string name)
{
	if (mVertexShaders[name])
		return mVertexShaders[name];
	else
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to get vertex shader " << name.c_str();
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
		return NULL;
	}
}

ID3D11PixelShader* ShaderHandler::GetPixelShader(std::string name)
{
	if (mPixelShaders[name])
		return mPixelShaders[name];
	else
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to get pixel shader " << name.c_str();
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
		return NULL;
	}
}

ID3D11GeometryShader* ShaderHandler::GetGeometryShader(std::string name)
{
	if (mGeometryShaders[name])
		return mGeometryShaders[name];
	else
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to get geometry shader " << name.c_str();
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
		return NULL;
	}
}


Shader* ShaderHandler::GetShader(std::string name)
{
	for (UINT i = 0; i < mShaders.size(); ++i)
	{
		if (mShaders[i]->Name == name)
		{
			return mShaders[i];
		}
	}

	std::wstringstream ErrorMsg;
	ErrorMsg << "Failed to get shader " << name.c_str();
	MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	return NULL;
}
#pragma endregion ShaderHandler

//----------------------------------------------------------------------------
// Basic shader
//----------------------------------------------------------------------------
#pragma region BasicShader
BasicShader::BasicShader(){}

BasicShader::~BasicShader()
{
	if (vs_cPerObjBuffer)
		vs_cPerObjBuffer->Release();
	if (ps_cPerObjBuffer)
		ps_cPerObjBuffer->Release();
	if (ps_cPerFrameBuffer)
		ps_cPerFrameBuffer->Release();
}

bool BasicShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffer
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&vs_cPerObjBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerObjBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerObjBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&ps_cPerObjBufferVariables, sizeof(PS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC ps_cbDesc;
	ps_cbDesc.ByteWidth = sizeof(PS_CPEROBJBUFFER);
	ps_cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	ps_cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ps_cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ps_cbDesc.MiscFlags = 0;
	ps_cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA PS_InitData;
	PS_InitData.pSysMem = &ps_cPerObjBufferVariables;
	PS_InitData.SysMemPitch = 0;
	PS_InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&ps_cbDesc, &PS_InitData, &ps_cPerObjBuffer);

	// PER FRAME BUFFER
	ZeroMemory(&ps_cPerFrameBufferVariables, sizeof(PS_CPERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC ps_cPerFramebDesc;
	ps_cPerFramebDesc.ByteWidth = sizeof(PS_CPERFRAMEBUFFER);
	ps_cPerFramebDesc.Usage = D3D11_USAGE_DYNAMIC;
	ps_cPerFramebDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ps_cPerFramebDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ps_cPerFramebDesc.MiscFlags = 0;
	ps_cPerFramebDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA PS_InitData2;
	PS_InitData2.pSysMem = &ps_cPerFrameBufferVariables;
	PS_InitData2.SysMemPitch = 0;
	PS_InitData2.SysMemSlicePitch = 0;

	device->CreateBuffer(&ps_cPerFramebDesc, &PS_InitData2, &ps_cPerFrameBuffer);

	// Set input layout
	mInputLayout = inputLayout;

	return true;
}

bool BasicShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);

	return true;
}

void BasicShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr->world = mBufferCache.vsPerObjBuffer.world;
	dataPtr->worldViewProj = mBufferCache.vsPerObjBuffer.worldViewProj;
	//dataPtr->worldViewProjTex = mBufferCache.vsBuffer.worldViewProjTex;
	dataPtr->worldInvTranspose = mBufferCache.vsPerObjBuffer.worldInvTranspose;
	dataPtr->texTransform = mBufferCache.vsPerObjBuffer.texTransform;
	dataPtr->shadowTransform = mBufferCache.vsPerObjBuffer.shadowTransform;

	dc->Unmap(vs_cPerObjBuffer, 0);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr2->mat = mBufferCache.psPerObjBuffer.mat;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);
	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

void BasicShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Pixel shader per frame buffer
	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPERFRAMEBUFFER* dataPtr3 = (PS_CPERFRAMEBUFFER*)mappedResource.pData;

	dataPtr3->numDirLights = mBufferCache.psPerFrameBuffer.numDirLights;
	dataPtr3->padding2 = 0;
	dataPtr3->padding3 = 0;
	dataPtr3->padding4 = 0;

	for (UINT i = 0; i < mBufferCache.psPerFrameBuffer.numDirLights; ++i)
		dataPtr3->dirLights[i] = mBufferCache.psPerFrameBuffer.dirLights[i];

	dataPtr3->gEyePosW = mBufferCache.psPerFrameBuffer.gEyePosW;
	dataPtr3->padding = 0.0f;

	dataPtr3->numPointLights = mBufferCache.psPerFrameBuffer.numPointLights;
	dataPtr3->padding5 = 0;
	dataPtr3->padding6 = 0;
	dataPtr3->padding7 = 0;

	for (UINT j = 0; j < mBufferCache.psPerFrameBuffer.numPointLights; ++j)
		dataPtr3->pointLights[j] = mBufferCache.psPerFrameBuffer.pointLights[j];

	dataPtr3->numSpotLights = mBufferCache.psPerFrameBuffer.numSpotLights;
	dataPtr3->padding8 = 0;
	dataPtr3->padding9 = 0;
	dataPtr3->padding10 = 0;

	for (UINT j = 0; j < mBufferCache.psPerFrameBuffer.numSpotLights; ++j)
		dataPtr3->spotLights[j] = mBufferCache.psPerFrameBuffer.spotLights[j];

	dc->Unmap(ps_cPerFrameBuffer, 0);

	dc->PSSetConstantBuffers(1, 1, &ps_cPerFrameBuffer);
}

void BasicShader::SetWorldViewProjTex(ID3D11DeviceContext* dc,
	XMMATRIX& world,
	XMMATRIX& viewProj,
	XMMATRIX& tex)
{
	XMMATRIX worldViewProj = XMMatrixMultiply(world, viewProj);
	XMMATRIX worldViewProjTex = XMMatrixMultiply(worldViewProj, tex);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	mBufferCache.vsPerObjBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsPerObjBuffer.worldInvTranspose = worldInvTranspose;
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(worldViewProj);
	//mBufferCache.vsBuffer.worldViewProjTex = worldViewProjTex;
	mBufferCache.vsPerObjBuffer.texTransform = XMMatrixTranspose(texTransform);
}

void BasicShader::SetEyePosW(ID3D11DeviceContext* dc, XMFLOAT3 eyePosW)
{
	mBufferCache.psPerFrameBuffer.gEyePosW = eyePosW;
}

void BasicShader::SetMaterial(ID3D11DeviceContext* dc, const Material& mat)
{
	mBufferCache.psPerObjBuffer.mat = mat;
}

void BasicShader::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{	
	dc->PSSetShaderResources(0, 1, &tex);
}

void BasicShader::SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* shadowMap)
{
	dc->PSSetShaderResources(1, 1, &shadowMap);
}

bool BasicShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void BasicShader::SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight pointLights[])
{
	mBufferCache.psPerFrameBuffer.numPointLights = numPointLights;

	for (UINT i = 0; i < numPointLights; ++i)
		mBufferCache.psPerFrameBuffer.pointLights[i] = pointLights[i];
}

void BasicShader::SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirectionalLight dirLights[])
{
	mBufferCache.psPerFrameBuffer.numDirLights = numDirLights;

	for (UINT i = 0; i < numDirLights; ++i)
		mBufferCache.psPerFrameBuffer.dirLights[i] = dirLights[i];
}

void BasicShader::SetShadowTransform(ID3D11DeviceContext* dc, const XMMATRIX& shadowTransform)
{
	mBufferCache.vsPerObjBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

#pragma endregion BasicShaderEnd

void BasicShader::SetSpotLights(ID3D11DeviceContext* dc, UINT numSpotLights, SpotLight spotLights[])
{
	mBufferCache.psPerFrameBuffer.numSpotLights = numSpotLights;

	for (UINT i = 0; i < numSpotLights; ++i)
		mBufferCache.psPerFrameBuffer.spotLights[i] = spotLights[i];
}

#pragma endregion BasicShader

#pragma region ShadowShader

ShadowShader::ShadowShader()
{

}

ShadowShader::~ShadowShader()
{
	/*
	if (mVertexShader)
		mVertexShader->Release();
	if (mPixelShader)
		mPixelShader->Release();
	*/

	if (vs_cBuffer)
		vs_cBuffer->Release();
}

bool ShadowShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool ShadowShader::BindVertexShader(ID3D11VertexShader* vShader)
{
	mVertexShader = vShader;

	return true;
}

bool ShadowShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	ZeroMemory(&vs_cBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool ShadowShader::SetActive(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);
	
	//test
	//dc->OMSetDepthStencilState(DSState, 0);

	return true;
}

void ShadowShader::setLightWVP(ID3D11DeviceContext* dc, const XMMATRIX& lgwp)
{
	mBufferCache.vsBuffer.lightWorldViewProj = XMMatrixTranspose(lgwp);
}

void ShadowShader::updatePerObj(ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dc->Map(vs_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

	//dataPtr->lightWorldViewProj = mBufferCache.vsBuffer.lightWorldViewProj;
	*dataPtr = mBufferCache.vsBuffer;

	dc->Unmap(vs_cBuffer, 0);
	dc->VSSetConstantBuffers(0, 1, &vs_cBuffer);
}

#pragma endregion ShadowShaderEnd


//-----------------------------------------------------------------------------------------------
// Sky shader
//-----------------------------------------------------------------------------------------------
#pragma region SkyShader
SkyShader::SkyShader(){}

SkyShader::~SkyShader()
{
	if (vs_cPerFrameBuffer)
		vs_cPerFrameBuffer->Release();
}

bool SkyShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SkyShader::SetWorldViewProj(const XMMATRIX& worldViewProj)
{
	mBufferCache.vsBuffer.WorldViewProj = XMMatrixTranspose(worldViewProj);
}

void SkyShader::SetCubeMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* cubeMap)
{
	dc->PSSetShaderResources(0, 1, &cubeMap);
}

void SkyShader::Update(ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dc->Map(vs_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPERFRAMEBUFFER* dataPtr = (VS_CPERFRAMEBUFFER*)mappedResource.pData;

	//dataPtr->WorldViewProj = mBufferCache.vsBuffer.WorldViewProj;
	*dataPtr = mBufferCache.vsBuffer;

	dc->Unmap(vs_cPerFrameBuffer, 0);
	dc->VSSetConstantBuffers(0, 1, &vs_cPerFrameBuffer);
}

bool SkyShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	ZeroMemory(&vs_cPerFrameBufferVariables, sizeof(VS_CPERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerFrameBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool SkyShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);

	return true;
}
#pragma endregion SkyShader

//-----------------------------------------------------------------------------------------------
// Normal mapping skinned shader
//-----------------------------------------------------------------------------------------------
#pragma region NormalMappedSkinned
void NormalMappedSkinned::SetBoneTransforms(ID3D11DeviceContext* dc, const XMFLOAT4X4 boneTransforms[], UINT numTransforms)
{
	for (UINT i = 0; i < numTransforms; ++i)
	{
		mBufferCache.vsSkinBuffer.boneTransforms[i] = XMLoadFloat4x4(&boneTransforms[i]);
		mBufferCache.vsSkinBuffer.boneTransforms[i] = XMMatrixTranspose(mBufferCache.vsSkinBuffer.boneTransforms[i]);
	}

	mBufferCache.vsSkinBuffer.numBoneTransforms = numTransforms;
}

void NormalMappedSkinned::SetEyePosW(ID3D11DeviceContext* dc, XMFLOAT3 eyePosW)
{
	mBufferCache.psPerFrameBuffer.gEyePosW = eyePosW;
}

void NormalMappedSkinned::SetMaterial(ID3D11DeviceContext* dc, const Material& mat)
{
	mBufferCache.psBuffer.mat = mat;
}

void NormalMappedSkinned::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void NormalMappedSkinned::SetNormalMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void NormalMappedSkinned::SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(2, 1, &tex);
}

void NormalMappedSkinned::SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight pointLights[])
{
	mBufferCache.psPerFrameBuffer.numPointLights = numPointLights;

	for (UINT i = 0; i < numPointLights; ++i)
		mBufferCache.psPerFrameBuffer.pointLights[i] = pointLights[i];
}

void NormalMappedSkinned::SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirectionalLight dirLights[])
{
	mBufferCache.psPerFrameBuffer.numDirLights = numDirLights;

	for (UINT i = 0; i < numDirLights; ++i)
		mBufferCache.psPerFrameBuffer.dirLights[i] = dirLights[i];
}

void NormalMappedSkinned::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr->world = mBufferCache.vsBuffer.world;
	dataPtr->worldViewProj = mBufferCache.vsBuffer.worldViewProj;
	//dataPtr->worldViewProjTex = mBufferCache.vsBuffer.worldViewProjTex;
	dataPtr->worldInvTranspose = mBufferCache.vsBuffer.worldInvTranspose;
	dataPtr->texTransform = mBufferCache.vsBuffer.texTransform;
	dataPtr->shadowTransform = mBufferCache.vsBuffer.shadowTransform;

	dc->Unmap(vs_cBuffer, 0);

	// Vertex shader per obj skinned buffer
	dc->Map(vs_cSkinnedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CSKINNEDBUFFER* dataPtr3 = (VS_CSKINNEDBUFFER*)mappedResource.pData;

	for (UINT i = 0; i < mBufferCache.vsSkinBuffer.numBoneTransforms; ++i)
		dataPtr3->boneTransforms[i] = mBufferCache.vsSkinBuffer.boneTransforms[i];

	dataPtr3->numBoneTransforms = mBufferCache.vsSkinBuffer.numBoneTransforms;
	dataPtr3->padding = 0;
	dataPtr3->padding2 = 0;
	dataPtr3->padding3 = 0;

	dc->Unmap(vs_cSkinnedBuffer, 0);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr2->mat = mBufferCache.psBuffer.mat;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cBuffer);
	dc->VSSetConstantBuffers(1, 1, &vs_cSkinnedBuffer);
	dc->PSSetConstantBuffers(1, 1, &ps_cPerObjBuffer);
}

void NormalMappedSkinned::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Pixel shader per frame buffer
	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPERFRAMEBUFFER* dataPtr3 = (PS_CPERFRAMEBUFFER*)mappedResource.pData;

	dataPtr3->numDirLights = mBufferCache.psPerFrameBuffer.numDirLights;
	dataPtr3->padding2 = 0;
	dataPtr3->padding3 = 0;
	dataPtr3->padding4 = 0;

	for (UINT i = 0; i < mBufferCache.psPerFrameBuffer.numDirLights; ++i)
		dataPtr3->dirLights[i] = mBufferCache.psPerFrameBuffer.dirLights[i];

	dataPtr3->gEyePosW = mBufferCache.psPerFrameBuffer.gEyePosW;
	dataPtr3->padding = 0.0f;

	dataPtr3->numPointLights = mBufferCache.psPerFrameBuffer.numPointLights;
	dataPtr3->padding5 = 0;
	dataPtr3->padding6 = 0;
	dataPtr3->padding7 = 0;

	for (UINT j = 0; j < mBufferCache.psPerFrameBuffer.numPointLights; ++j)
		dataPtr3->pointLights[j] = mBufferCache.psPerFrameBuffer.pointLights[j];

	dataPtr3->numSpotLights = mBufferCache.psPerFrameBuffer.numSpotLights;
	dataPtr3->padding8 = 0;
	dataPtr3->padding9 = 0;
	dataPtr3->padding10 = 0;

	for (UINT j = 0; j < mBufferCache.psPerFrameBuffer.numSpotLights; ++j)
		dataPtr3->spotLights[j] = mBufferCache.psPerFrameBuffer.spotLights[j];

	dc->Unmap(ps_cPerFrameBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerFrameBuffer);
}

NormalMappedSkinned::NormalMappedSkinned()
{

}

NormalMappedSkinned::~NormalMappedSkinned()
{
	if (vs_cBuffer)
		vs_cBuffer->Release();
	if (ps_cPerObjBuffer)
		ps_cPerObjBuffer->Release();
	if (ps_cPerFrameBuffer)
		ps_cPerFrameBuffer->Release();
	if (vs_cSkinnedBuffer)
		vs_cSkinnedBuffer->Release();
}

bool NormalMappedSkinned::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&vs_cBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cBuffer);

	// Skinned buffer
	ZeroMemory(&vs_cSkinnedBufferVariables, sizeof(VS_CSKINNEDBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbSkinDesc;
	cbSkinDesc.ByteWidth = sizeof(VS_CSKINNEDBUFFER);
	cbSkinDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbSkinDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbSkinDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbSkinDesc.MiscFlags = 0;
	cbSkinDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA SkinInitData;
	SkinInitData.pSysMem = &vs_cSkinnedBufferVariables;
	SkinInitData.SysMemPitch = 0;
	SkinInitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbSkinDesc, &SkinInitData, &vs_cSkinnedBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	ZeroMemory(&ps_cPerObjBufferVariables, sizeof(PS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC ps_cbDesc;
	ps_cbDesc.ByteWidth = sizeof(PS_CPEROBJBUFFER);
	ps_cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	ps_cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ps_cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ps_cbDesc.MiscFlags = 0;
	ps_cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA PS_InitData;
	PS_InitData.pSysMem = &ps_cPerObjBufferVariables;
	PS_InitData.SysMemPitch = 0;
	PS_InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&ps_cbDesc, &PS_InitData, &ps_cPerObjBuffer);

	// PER FRAME BUFFER
	ZeroMemory(&ps_cPerFrameBufferVariables, sizeof(PS_CPERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC ps_cPerFramebDesc;
	ps_cPerFramebDesc.ByteWidth = sizeof(PS_CPERFRAMEBUFFER);
	ps_cPerFramebDesc.Usage = D3D11_USAGE_DYNAMIC;
	ps_cPerFramebDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ps_cPerFramebDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ps_cPerFramebDesc.MiscFlags = 0;
	ps_cPerFramebDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA PS_InitData2;
	PS_InitData2.pSysMem = &ps_cPerFrameBufferVariables;
	PS_InitData2.SysMemPitch = 0;
	PS_InitData2.SysMemSlicePitch = 0;

	device->CreateBuffer(&ps_cPerFramebDesc, &PS_InitData2, &ps_cPerFrameBuffer);

	// Set input layout
	mInputLayout = inputLayout;

	return true;
}

bool NormalMappedSkinned::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool NormalMappedSkinned::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mComparisonSS);

	return true;
}

void NormalMappedSkinned::SetWorldViewProjTex(ID3D11DeviceContext* dc, XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	XMMATRIX worldViewProj = XMMatrixMultiply(world, viewProj);
	XMMATRIX worldViewProjTex = XMMatrixMultiply(worldViewProj, tex);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	mBufferCache.vsBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsBuffer.worldInvTranspose = worldInvTranspose;
	mBufferCache.vsBuffer.worldViewProj = XMMatrixTranspose(worldViewProj);
	//mBufferCache.vsBuffer.worldViewProjTex = worldViewProjTex;
	mBufferCache.vsBuffer.texTransform = XMMatrixTranspose(texTransform);
}

void NormalMappedSkinned::SetSpotLights(ID3D11DeviceContext* dc, UINT numSpotLights, SpotLight spotLights[])
{
	mBufferCache.psPerFrameBuffer.numSpotLights = numSpotLights;

	for (UINT i = 0; i < numSpotLights; ++i)
	{
		mBufferCache.psPerFrameBuffer.spotLights[i] = spotLights[i];
	}
}
void NormalMappedSkinned::SetShadowStransform(ID3D11DeviceContext* dc, XMMATRIX& shadowTransform)
{
	mBufferCache.vsBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

#pragma endregion NormalMapSkinnedEnd

#pragma region SkinnedShadowShader

SkinnedShadowShader::SkinnedShadowShader()
{

}

SkinnedShadowShader::~SkinnedShadowShader()
{
	if (vs_cBuffer)
		vs_cBuffer->Release();
	if (vs_cSkinnedBuffer)
		vs_cSkinnedBuffer->Release();
}

bool SkinnedShadowShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool SkinnedShadowShader::BindVertexShader(ID3D11VertexShader* vShader)
{
	mVertexShader = vShader;

	return true;
}

bool SkinnedShadowShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&vs_cBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cBuffer);

	// Skinned buffer
	ZeroMemory(&vs_cSkinnedBufferVariables, sizeof(VS_CSKINNEDBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbSkinDesc;
	cbSkinDesc.ByteWidth = sizeof(VS_CSKINNEDBUFFER);
	cbSkinDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbSkinDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbSkinDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbSkinDesc.MiscFlags = 0;
	cbSkinDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA SkinInitData;
	SkinInitData.pSysMem = &vs_cSkinnedBufferVariables;
	SkinInitData.SysMemPitch = 0;
	SkinInitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbSkinDesc, &SkinInitData, &vs_cSkinnedBuffer);

	// Set input layout
	mInputLayout = inputLayout;

	return true;
}

void SkinnedShadowShader::SetBoneTransforms(ID3D11DeviceContext* dc, const XMFLOAT4X4 boneTransforms[], UINT numTransforms)
{
	for (UINT i = 0; i < numTransforms; ++i)
	{
		mBufferCache.vsSkinBuffer.boneTransforms[i] = XMLoadFloat4x4(&boneTransforms[i]);
		mBufferCache.vsSkinBuffer.boneTransforms[i] = XMMatrixTranspose(mBufferCache.vsSkinBuffer.boneTransforms[i]);
	}

	mBufferCache.vsSkinBuffer.numBoneTransforms = numTransforms;
}

void SkinnedShadowShader::SetLightWVP(ID3D11DeviceContext* dc, XMMATRIX& lwvp)
{
	mBufferCache.vsBuffer.lightWVP = XMMatrixTranspose(lwvp);
}

void SkinnedShadowShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

	//dataPtr->lightWVP = mBufferCache.vsBuffer.lightWVP;
	*dataPtr = mBufferCache.vsBuffer;

	dc->Unmap(vs_cBuffer, 0);

	// Vertex shader per obj skinned buffer
	dc->Map(vs_cSkinnedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CSKINNEDBUFFER* dataPtr3 = (VS_CSKINNEDBUFFER*)mappedResource.pData;

// 	for (UINT i = 0; i < mBufferCache.vsSkinBuffer.numBoneTransforms; ++i)
// 		dataPtr3->boneTransforms[i] = mBufferCache.vsSkinBuffer.boneTransforms[i];
// 
// 	dataPtr3->numBoneTransforms = mBufferCache.vsSkinBuffer.numBoneTransforms;
// 	dataPtr3->padding = 0;
// 	dataPtr3->padding2 = 0;
// 	dataPtr3->padding3 = 0;
	*dataPtr3 = mBufferCache.vsSkinBuffer;

	dc->Unmap(vs_cSkinnedBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cBuffer);
	dc->VSSetConstantBuffers(1, 1, &vs_cSkinnedBuffer);
}

bool SkinnedShadowShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	return true;
}

#pragma endregion SkinnedShadowShaderEnd

#pragma endregion NormalMapSkinned

BasicDeferredShader::BasicDeferredShader()
{

}

BasicDeferredShader::~BasicDeferredShader()
{
	if (vs_cPerObjBuffer)
		vs_cPerObjBuffer->Release();
	if (ps_cPerObjBuffer)
		ps_cPerObjBuffer->Release();
}

bool BasicDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&vs_cPerObjBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerObjBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerObjBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&ps_cPerObjBufferVariables, sizeof(PS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc2;
	cbDesc2.ByteWidth = sizeof(PS_CPEROBJBUFFER);
	cbDesc2.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc2.MiscFlags = 0;
	cbDesc2.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData2;
	InitData2.pSysMem = &ps_cPerObjBufferVariables;
	InitData2.SysMemPitch = 0;
	InitData2.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc2, &InitData2, &ps_cPerObjBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool BasicDeferredShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool BasicDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);

	return true;
}

void BasicDeferredShader::SetType(int type)
{
	mBufferCache.psPerObjBuffer.type = type;
}

void BasicDeferredShader::SetWorldViewProjTex(XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	mBufferCache.vsPerObjBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
	mBufferCache.vsPerObjBuffer.worldInvTranspose = XMMatrixTranspose(MathHelper::InverseTranspose(world));
	mBufferCache.vsPerObjBuffer.texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

void BasicDeferredShader::SetMaterial(const Material& mat)
{
	mBufferCache.psPerObjBuffer.mat = mat;
}

void BasicDeferredShader::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void BasicDeferredShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

// 	dataPtr->world = mBufferCache.vsPerObjBuffer.world;
// 	dataPtr->worldViewProj = mBufferCache.vsPerObjBuffer.worldViewProj;
// 	//dataPtr->worldViewProjTex = mBufferCache.vsBuffer.worldViewProjTex;
// 	dataPtr->worldInvTranspose = mBufferCache.vsPerObjBuffer.worldInvTranspose;
// 	dataPtr->texTransform = mBufferCache.vsPerObjBuffer.texTransform;
	*dataPtr = mBufferCache.vsPerObjBuffer;

	dc->Unmap(vs_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	//dataPtr2->mat = mBufferCache.psPerObjBuffer.mat;
	*dataPtr2 = mBufferCache.psPerObjBuffer;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

void BasicDeferredShader::SetShadowTransformLightViewProj(XMMATRIX& shadowTransform, XMMATRIX& lightView, XMMATRIX& lightProj)
{
	mBufferCache.vsPerObjBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

void BasicDeferredShader::SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void BasicDeferredShader::SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj)
{
	mBufferCache.vsPerObjBuffer.prevWorldViewProj = XMMatrixTranspose(XMMatrixMultiply(prevWorld, prevViewProj));
}

BasicDeferredSkinnedShader::BasicDeferredSkinnedShader()
{

}

BasicDeferredSkinnedShader::~BasicDeferredSkinnedShader()
{
	if (vs_cPerObjBuffer)
		vs_cPerObjBuffer->Release();
	if (vs_cSkinnedBuffer)
		vs_cSkinnedBuffer->Release();
	if (ps_cPerObjBuffer)
		ps_cPerObjBuffer->Release();
}

bool BasicDeferredSkinnedShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&vs_cPerObjBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerObjBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerObjBuffer);

	// SKINNED BUFFER
	ZeroMemory(&vs_cSkinnedBufferVariables, sizeof(VS_CSKINNEDBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbSkinDesc;
	cbSkinDesc.ByteWidth = sizeof(VS_CSKINNEDBUFFER);
	cbSkinDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbSkinDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbSkinDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbSkinDesc.MiscFlags = 0;
	cbSkinDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA SkinInitData;
	SkinInitData.pSysMem = &vs_cSkinnedBufferVariables;
	SkinInitData.SysMemPitch = 0;
	SkinInitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbSkinDesc, &SkinInitData, &vs_cSkinnedBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&ps_cPerObjBufferVariables, sizeof(PS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc2;
	cbDesc2.ByteWidth = sizeof(PS_CPEROBJBUFFER);
	cbDesc2.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc2.MiscFlags = 0;
	cbDesc2.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData2;
	InitData2.pSysMem = &ps_cPerObjBufferVariables;
	InitData2.SysMemPitch = 0;
	InitData2.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc2, &InitData2, &ps_cPerObjBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool BasicDeferredSkinnedShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool BasicDeferredSkinnedShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);

	return true;
}

void BasicDeferredSkinnedShader::SetWorldViewProjTex(XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	mBufferCache.vsPerObjBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
	mBufferCache.vsPerObjBuffer.worldInvTranspose = XMMatrixTranspose(MathHelper::InverseTranspose(world));
	mBufferCache.vsPerObjBuffer.texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

void BasicDeferredSkinnedShader::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void BasicDeferredSkinnedShader::SetBoneTransforms(const XMFLOAT4X4 boneTransforms[], UINT numTransforms)
{
	for (UINT i = 0; i < numTransforms; ++i)
	{
		mBufferCache.vsSkinnedBuffer.boneTransforms[i] = XMLoadFloat4x4(&boneTransforms[i]);
		mBufferCache.vsSkinnedBuffer.boneTransforms[i] = XMMatrixTranspose(mBufferCache.vsSkinnedBuffer.boneTransforms[i]);
	}

	mBufferCache.vsSkinnedBuffer.numBoneTransforms = numTransforms;
}

void BasicDeferredSkinnedShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

// 	dataPtr->world = mBufferCache.vsPerObjBuffer.world;
// 	dataPtr->worldViewProj = mBufferCache.vsPerObjBuffer.worldViewProj;
// 	//dataPtr->worldViewProjTex = mBufferCache.vsBuffer.worldViewProjTex;
// 	dataPtr->worldInvTranspose = mBufferCache.vsPerObjBuffer.worldInvTranspose;
// 	dataPtr->texTransform = mBufferCache.vsPerObjBuffer.texTransform;

	*dataPtr = mBufferCache.vsPerObjBuffer;

	dc->Unmap(vs_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);

	// Vertex shader per obj skinned buffer
	dc->Map(vs_cSkinnedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CSKINNEDBUFFER* dataPtr3 = (VS_CSKINNEDBUFFER*)mappedResource.pData;

// 	for (UINT i = 0; i < mBufferCache.vsSkinnedBuffer.numBoneTransforms; ++i)
// 		dataPtr3->boneTransforms[i] = mBufferCache.vsSkinnedBuffer.boneTransforms[i];
// 
// 	dataPtr3->numBoneTransforms = mBufferCache.vsSkinnedBuffer.numBoneTransforms;
// 	dataPtr3->padding = 0;
// 	dataPtr3->padding2 = 0;
// 	dataPtr3->padding3 = 0;
	*dataPtr3 = mBufferCache.vsSkinnedBuffer;

	dc->Unmap(vs_cSkinnedBuffer, 0);

	dc->VSSetConstantBuffers(1, 1, &vs_cSkinnedBuffer);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	//dataPtr2->mat = mBufferCache.psPerObjBuffer.mat;
	*dataPtr2 = mBufferCache.psPerObjBuffer;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

void BasicDeferredSkinnedShader::SetMaterial(const Material& mat)
{
	mBufferCache.psPerObjBuffer.mat = mat;
}

void BasicDeferredSkinnedShader::SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void BasicDeferredSkinnedShader::SetShadowTransform(XMMATRIX& shadowTransform)
{
	mBufferCache.vsPerObjBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

void BasicDeferredSkinnedShader::SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj)
{
	mBufferCache.vsPerObjBuffer.prevWorldViewProj = XMMatrixTranspose(XMMatrixMultiply(prevWorld, prevViewProj));
}

void LightDeferredShader::SetEyePosW(XMFLOAT3 eyePosW)
{
	mBufferCache.psPerFrameBuffer.gEyePosW = eyePosW;
}

bool LightDeferredShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void LightDeferredShader::SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight pointLights[])
{
	mBufferCache.psPerFrameBuffer.numPointLights = numPointLights;

	for (UINT i = 0; i < numPointLights; ++i)
		mBufferCache.psPerFrameBuffer.pointLights[i] = pointLights[i];
}

void LightDeferredShader::SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirectionalLight dirLights[])
{
	mBufferCache.psPerFrameBuffer.numDirLights = numDirLights;

	for (UINT i = 0; i < numDirLights; ++i)
		mBufferCache.psPerFrameBuffer.dirLights[i] = dirLights[i];
}

void LightDeferredShader::SetSpotLights(ID3D11DeviceContext* dc, UINT numSpotLights, SpotLight spotLights[])
{
	mBufferCache.psPerFrameBuffer.numSpotLights = numSpotLights;

	for (UINT i = 0; i < numSpotLights; ++i)
		mBufferCache.psPerFrameBuffer.spotLights[i] = spotLights[i];
}

LightDeferredShader::LightDeferredShader()
{

}

LightDeferredShader::~LightDeferredShader()
{
	if (vs_cPerObjBuffer)
		vs_cPerObjBuffer->Release();
	if (ps_cPerFrameBuffer)
		ps_cPerFrameBuffer->Release();
}

bool LightDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&mBufferCache.vsPerObjBuffer, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &mBufferCache.vsPerObjBuffer;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerObjBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER FRAME BUFFER
	ZeroMemory(&mBufferCache.psPerFrameBuffer, sizeof(PS_CPERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC ps_cPerFramebDesc;
	ps_cPerFramebDesc.ByteWidth = sizeof(PS_CPERFRAMEBUFFER);
	ps_cPerFramebDesc.Usage = D3D11_USAGE_DYNAMIC;
	ps_cPerFramebDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ps_cPerFramebDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ps_cPerFramebDesc.MiscFlags = 0;
	ps_cPerFramebDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA PS_InitData2;
	PS_InitData2.pSysMem = &mBufferCache.psPerFrameBuffer;
	PS_InitData2.SysMemPitch = 0;
	PS_InitData2.SysMemSlicePitch = 0;

	device->CreateBuffer(&ps_cPerFramebDesc, &PS_InitData2, &ps_cPerFrameBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool LightDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);
	dc->PSSetSamplers(3, 1, &RenderStates::mPointClampedSS);

	return true;
}

void LightDeferredShader::SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void LightDeferredShader::SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void LightDeferredShader::SetSpecularTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(2, 1, &tex);
}

// void LightDeferredShader::SetPositionTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
// {
// 	dc->PSSetShaderResources(3, 1, &tex);
// }

void LightDeferredShader::SetSSAOTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(4, 1, &tex);
}

void LightDeferredShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

// 	dataPtr->worldViewProj = mBufferCache.vsPerObjBuffer.worldViewProj;
// 	dataPtr->shadowTransform = mBufferCache.vsPerObjBuffer.shadowTransform;

	*dataPtr = mBufferCache.vsPerObjBuffer;

	dc->Unmap(vs_cPerObjBuffer, 0);
	
	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);
}

void LightDeferredShader::SetWorldViewProj(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj)
{
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
}

void LightDeferredShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Pixel shader per frame buffer
	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPERFRAMEBUFFER* dataPtr3 = (PS_CPERFRAMEBUFFER*)mappedResource.pData;
// 
// 	dataPtr3->numDirLights = mBufferCache.psPerFrameBuffer.numDirLights;
// 	dataPtr3->padding2 = 0;
// 	dataPtr3->padding3 = 0;
// 	dataPtr3->padding4 = 0;
// 
// 	for (UINT i = 0; i < mBufferCache.psPerFrameBuffer.numDirLights; ++i)
// 		dataPtr3->dirLights[i] = mBufferCache.psPerFrameBuffer.dirLights[i];
// 
// 	dataPtr3->gEyePosW = mBufferCache.psPerFrameBuffer.gEyePosW;
// 	dataPtr3->padding = 0.0f;
// 
// 	dataPtr3->numPointLights = mBufferCache.psPerFrameBuffer.numPointLights;
// 	dataPtr3->padding5 = 0;
// 	dataPtr3->padding6 = 0;
// 	dataPtr3->padding7 = 0;
// 
// 	for (UINT j = 0; j < mBufferCache.psPerFrameBuffer.numPointLights; ++j)
// 		dataPtr3->pointLights[j] = mBufferCache.psPerFrameBuffer.pointLights[j];
// 
// 	dataPtr3->numSpotLights = mBufferCache.psPerFrameBuffer.numSpotLights;
// 	dataPtr3->padding8 = 0;
// 	dataPtr3->padding9 = 0;
// 	dataPtr3->padding10 = 0;
// 
// 	for (UINT j = 0; j < mBufferCache.psPerFrameBuffer.numSpotLights; ++j)
// 		dataPtr3->spotLights[j] = mBufferCache.psPerFrameBuffer.spotLights[j];
// 
// 	dataPtr3->shadowTransform = mBufferCache.psPerFrameBuffer.shadowTransform;
// 	dataPtr3->cameraViewMatrix = mBufferCache.psPerFrameBuffer.cameraViewMatrix;
// 	dataPtr3->cameraWorldMatrix = mBufferCache.psPerFrameBuffer.cameraWorldMatrix;
// 
// 	dataPtr3->lightWorldMatrix = mBufferCache.psPerFrameBuffer.lightWorldMatrix;
// 	dataPtr3->lightViewMatrix = mBufferCache.psPerFrameBuffer.lightViewMatrix;
// 	dataPtr3->lightProjMatrix = mBufferCache.psPerFrameBuffer.lightProjMatrix;

	*dataPtr3 = mBufferCache.psPerFrameBuffer;

	dc->Unmap(ps_cPerFrameBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerFrameBuffer);
}

void LightDeferredShader::SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(6, 1, &tex);
}

void LightDeferredShader::SetShadowTransform(XMMATRIX& shadowTransform)
{
	mBufferCache.psPerFrameBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
	mBufferCache.vsPerObjBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

void LightDeferredShader::SetCameraViewProjMatrix(XMMATRIX& camViewMatrix, XMMATRIX& proj)
{
	mBufferCache.psPerFrameBuffer.cameraViewMatrix = XMMatrixTranspose(camViewMatrix);
	mBufferCache.psPerFrameBuffer.cameraInvViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, camViewMatrix));
	mBufferCache.psPerFrameBuffer.cameraProjMatrix = XMMatrixTranspose(proj);

	XMMATRIX viewProj = XMMatrixMultiply(camViewMatrix, proj);
	XMMATRIX viewProjInv = XMMatrixInverse(nullptr, viewProj);

	mBufferCache.psPerFrameBuffer.camViewProjInv = XMMatrixTranspose(viewProjInv);

	mBufferCache.vsPerObjBuffer.viewProjInv = XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixMultiply(camViewMatrix, proj)));
}

void LightDeferredShader::SetCameraWorldMatrix(XMMATRIX& camWorldMatrix)
{
	mBufferCache.psPerFrameBuffer.cameraWorldMatrix = XMMatrixTranspose(camWorldMatrix);
}

void LightDeferredShader::SetLightWorldViewProj(XMMATRIX& lightWorld, XMMATRIX& lightView, XMMATRIX& lightProj)
{
	mBufferCache.psPerFrameBuffer.lightWorldMatrix = XMMatrixTranspose(lightWorld);
	mBufferCache.psPerFrameBuffer.lightViewMatrix = XMMatrixTranspose(lightView);
	mBufferCache.psPerFrameBuffer.lightProjMatrix = XMMatrixTranspose(lightProj);
	mBufferCache.psPerFrameBuffer.lightInvViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, lightView));

	mBufferCache.vsPerObjBuffer.lightViewProj = XMMatrixTranspose(XMMatrixMultiply(lightView, lightProj));
}

void LightDeferredShader::SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(5, 1, &tex);
}

void LightDeferredShader::SetFogProperties(int enableFogging, float heightFalloff, float heightOffset, float globalDensity, XMFLOAT4 fogColor)
{
	mBufferCache.psPerFrameBuffer.enableFogging = enableFogging;
	mBufferCache.psPerFrameBuffer.fogHeightFalloff = heightFalloff;
	mBufferCache.psPerFrameBuffer.fogHeightOffset = heightOffset;
	mBufferCache.psPerFrameBuffer.fogGlobalDensity = globalDensity;
	mBufferCache.psPerFrameBuffer.fogColor = fogColor;
}

void LightDeferredShader::SetVelocityTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(3, 1, &tex);
}

void LightDeferredShader::SetMotionBlurProperties(int enableMotionBlur)
{
	mBufferCache.psPerFrameBuffer.enableMotionBlur = enableMotionBlur;
}

void LightDeferredShader::SetFpsValues(float curFps, float targetFps)
{
	mBufferCache.psPerFrameBuffer.curFPS = curFps;
	mBufferCache.psPerFrameBuffer.targetFPS = targetFps;
}

void LightDeferredShader::SetBackgroundTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(6, 1, &tex);
}

void LightDeferredShader::SetSkipLighting(bool skipLighting)
{
	mBufferCache.psPerFrameBuffer.skipLighting = skipLighting;
}

void LightDeferredShader::SetIsTransparencyPass(bool isTransparencyPass)
{
	mBufferCache.psPerFrameBuffer.isTransparencyPass = isTransparencyPass;
}

SkyDeferredShader::SkyDeferredShader()
{

}

SkyDeferredShader::~SkyDeferredShader()
{
	if (vs_cPerFrameBuffer)
		vs_cPerFrameBuffer->Release();
}

bool SkyDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	ZeroMemory(&vs_cPerFrameBufferVariables, sizeof(VS_CPERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerFrameBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool SkyDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);

	return true;
}

bool SkyDeferredShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SkyDeferredShader::SetWorldViewProj(const XMMATRIX& worldViewProj)
{
	mBufferCache.vsBuffer.WorldViewProj = XMMatrixTranspose(worldViewProj);
}

void SkyDeferredShader::SetCubeMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* cubeMap)
{
	dc->PSSetShaderResources(0, 1, &cubeMap);
}

void SkyDeferredShader::Update(ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dc->Map(vs_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPERFRAMEBUFFER* dataPtr = (VS_CPERFRAMEBUFFER*)mappedResource.pData;

	//dataPtr->WorldViewProj = mBufferCache.vsBuffer.WorldViewProj;
	*dataPtr = mBufferCache.vsBuffer;

	dc->Unmap(vs_cPerFrameBuffer, 0);
	dc->VSSetConstantBuffers(0, 1, &vs_cPerFrameBuffer);
}

void SkyDeferredShader::SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj)
{
	mBufferCache.vsBuffer.prevWorldViewProj = XMMatrixTranspose(XMMatrixMultiply(prevWorld, prevViewProj));
}

#pragma region SSAOShader

SSAOShader::SSAOShader()
{
}

SSAOShader::~SSAOShader()
{
	if (ps_cPerFrameBuffer)
		ps_cPerFrameBuffer->Release();
}

bool SSAOShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	memset(&ps_cPerFrameBufferVariables, 0, sizeof(PS_CPERFRAMEBUFFER));

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(PS_CPERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	device->CreateBuffer(&cbDesc, NULL, &ps_cPerFrameBuffer);

	return true;
}

bool SSAOShader::SetActive(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(NULL);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mSSAODepthSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mLinearSS);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerFrameBuffer);

	return true;
}

void SSAOShader::Update(ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPERFRAMEBUFFER* dataPtr = (PS_CPERFRAMEBUFFER*)mappedResource.pData;
	*dataPtr = ps_cPerFrameBufferVariables;

	dc->Unmap(ps_cPerFrameBuffer, 0);
}

bool SSAOShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SSAOShader::SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void SSAOShader::SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void SSAOShader::SetRandomTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(2, 1, &tex);
}

void SSAOShader::SetInverseProjectionMatrix(const XMMATRIX& inverseProjectionMatrix)
{
	ps_cPerFrameBufferVariables.inverseProjectionMatrix = XMMatrixTranspose(inverseProjectionMatrix);
}

void SSAOShader::SetViewMatrix(const XMMATRIX& viewMatrix)
{
	ps_cPerFrameBufferVariables.viewMatrix = XMMatrixTranspose(viewMatrix);
}

void SSAOShader::SetZFar(float z_far)
{
	ps_cPerFrameBufferVariables.z_far = z_far;
}

void SSAOShader::SetParameters(float radius, float projection_factor, float bias, float contrast, float sigma)
{
	ps_cPerFrameBufferVariables.radius = radius;
	ps_cPerFrameBufferVariables.projection_factor = projection_factor;
	ps_cPerFrameBufferVariables.bias = bias;
	ps_cPerFrameBufferVariables.contrast = contrast;
	ps_cPerFrameBufferVariables.sigma = sigma;
}

#pragma endregion SSAOShader

#pragma region DepthOfFieldCoCShader

DepthOfFieldCoCShader::DepthOfFieldCoCShader()
{
}

DepthOfFieldCoCShader::~DepthOfFieldCoCShader()
{
	if (ps_cPerFrameBuffer)
		ps_cPerFrameBuffer->Release();
}

bool DepthOfFieldCoCShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	memset(&ps_cPerFrameBufferVariables, 0, sizeof(PS_CPERFRAMEBUFFER));

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(PS_CPERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	device->CreateBuffer(&cbDesc, NULL, &ps_cPerFrameBuffer);

	return true;
}

bool DepthOfFieldCoCShader::SetActive(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(NULL);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerFrameBuffer);

	return true;
}

void DepthOfFieldCoCShader::Update(ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPERFRAMEBUFFER* dataPtr = (PS_CPERFRAMEBUFFER*)mappedResource.pData;
	*dataPtr = ps_cPerFrameBufferVariables;

	dc->Unmap(ps_cPerFrameBuffer, 0);
}

bool DepthOfFieldCoCShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void DepthOfFieldCoCShader::SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void DepthOfFieldCoCShader::SetZNearFar(float z_near, float z_far)
{
	ps_cPerFrameBufferVariables.z_near = z_near;
	ps_cPerFrameBufferVariables.z_far = z_far;
}

void DepthOfFieldCoCShader::SetFocusPlanes(float near_blurry_plane, float near_sharp_plane, float far_sharp_plane, float far_blurry_plane)
{
	ps_cPerFrameBufferVariables.near_blurry_plane = near_blurry_plane;
	ps_cPerFrameBufferVariables.near_sharp_plane = near_sharp_plane;
	ps_cPerFrameBufferVariables.far_sharp_plane = far_sharp_plane;
	ps_cPerFrameBufferVariables.far_blurry_plane = far_blurry_plane;

	ps_cPerFrameBufferVariables.near_scale = 1.0f / -(near_blurry_plane - near_sharp_plane);
	ps_cPerFrameBufferVariables.far_scale = 1.0f / (far_blurry_plane - far_sharp_plane);
}

#pragma endregion DepthOfFieldCoCShader

#pragma region BlurShader

BlurShader::BlurShader()
{
}

BlurShader::~BlurShader()
{
	if (ps_cPerFrameBuffer)
		ps_cPerFrameBuffer->Release();
}

bool BlurShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	memset(&ps_cPerFrameBufferVariables, 0, sizeof(PS_CPERFRAMEBUFFER));

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(PS_CPERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	device->CreateBuffer(&cbDesc, NULL, &ps_cPerFrameBuffer);

	return true;
}

bool BlurShader::SetActive(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(NULL);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearClampedSS);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerFrameBuffer);

	return true;
}

void BlurShader::Update(ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPERFRAMEBUFFER* dataPtr = (PS_CPERFRAMEBUFFER*)mappedResource.pData;
	*dataPtr = ps_cPerFrameBufferVariables;

	dc->Unmap(ps_cPerFrameBuffer, 0);
}

bool BlurShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void BlurShader::SetFramebufferTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void BlurShader::SetInputTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void BlurShader::SetFramebufferSize(const XMFLOAT2 &framebufferSize)
{
	ps_cPerFrameBufferVariables.framebufferSize = framebufferSize;
}

void BlurShader::SetZNearFar(float z_near, float z_far)
{
	ps_cPerFrameBufferVariables.z_near = z_near;
	ps_cPerFrameBufferVariables.z_far = z_far;
}

#pragma endregion BlurShader

BasicDeferredMorphShader::BasicDeferredMorphShader()
{

}

BasicDeferredMorphShader::~BasicDeferredMorphShader()
{
	if (vs_cPerObjBuffer)
		vs_cPerObjBuffer->Release();
	if (ps_cPerObjBuffer)
		ps_cPerObjBuffer->Release();
}

bool BasicDeferredMorphShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&vs_cPerObjBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerObjBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerObjBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&ps_cPerObjBufferVariables, sizeof(PS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc2;
	cbDesc2.ByteWidth = sizeof(PS_CPEROBJBUFFER);
	cbDesc2.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc2.MiscFlags = 0;
	cbDesc2.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData2;
	InitData2.pSysMem = &ps_cPerObjBufferVariables;
	InitData2.SysMemPitch = 0;
	InitData2.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc2, &InitData2, &ps_cPerObjBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool BasicDeferredMorphShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool BasicDeferredMorphShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);

	return true;
}

void BasicDeferredMorphShader::SetWorldViewProjTex(XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	mBufferCache.vsPerObjBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
	mBufferCache.vsPerObjBuffer.worldInvTranspose = XMMatrixTranspose(MathHelper::InverseTranspose(world));
	mBufferCache.vsPerObjBuffer.texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

void BasicDeferredMorphShader::SetMaterial(const Material& mat)
{
	mBufferCache.psPerObjBuffer.mat = mat;
}

void BasicDeferredMorphShader::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void BasicDeferredMorphShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;
	*dataPtr = mBufferCache.vsPerObjBuffer;

	dc->Unmap(vs_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	*dataPtr2 = mBufferCache.psPerObjBuffer;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

void BasicDeferredMorphShader::SetWeights(XMFLOAT4 weights)
{
	mBufferCache.vsPerObjBuffer.weights = weights;
}

void BasicDeferredMorphShader::SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void BasicDeferredMorphShader::SetShadowTransform(XMMATRIX& shadowTransform)
{
	mBufferCache.vsPerObjBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

void BasicDeferredMorphShader::SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj)
{
	mBufferCache.vsPerObjBuffer.prevWorldViewProj = XMMatrixTranspose(XMMatrixMultiply(prevWorld, prevViewProj));
}

ShadowMorphShader::ShadowMorphShader()
{

}

ShadowMorphShader::~ShadowMorphShader()
{
	if (vs_cBuffer)
		vs_cBuffer->Release();
}

bool ShadowMorphShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&vs_cBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool ShadowMorphShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool ShadowMorphShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	return true;
}

void ShadowMorphShader::SetLightWVP(ID3D11DeviceContext* dc, XMMATRIX& lwvp)
{
	mBufferCache.vsBuffer.lightWVP = XMMatrixTranspose(lwvp);
}

void ShadowMorphShader::SetWeights(XMFLOAT4 weights)
{
	mBufferCache.vsBuffer.weights = weights;
}

void ShadowMorphShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;
	*dataPtr = mBufferCache.vsBuffer;

	dc->Unmap(vs_cBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cBuffer);
}
#pragma region CompositeShader

CompositeShader::CompositeShader()
{
}

CompositeShader::~CompositeShader()
{
}

bool CompositeShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	return true;
}

bool CompositeShader::SetActive(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(NULL);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearClampedSS);

	return true;
}

void CompositeShader::Update(ID3D11DeviceContext* dc)
{
}

bool CompositeShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void CompositeShader::SetFramebufferTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void CompositeShader::SetDoFCoCTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void CompositeShader::SetDoFFarFieldTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(2, 1, &tex);
}

#pragma endregion BlurShader

ParticleSystemShader::ParticleSystemShader()
{

}

ParticleSystemShader::~ParticleSystemShader()
{
	if (draw_GS_PerFrameBuffer)
		draw_GS_PerFrameBuffer->Release();

	if (streamOut_GS_PerFrameBuffer)
		streamOut_GS_PerFrameBuffer->Release();

	if (draw_VS_InitBuffer)
		draw_VS_InitBuffer->Release();
}

bool ParticleSystemShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Geometry shader buffers
	//------------------------
	// DRAW GS PER FRAME
	ZeroMemory(&draw_GS_PerFrameBufferVariables, sizeof(DRAW_GS_PERFRAMEBUFFER));

	// Constant values
	draw_GS_PerFrameBufferVariables.quadTexC[0].x = 0.0f;
	draw_GS_PerFrameBufferVariables.quadTexC[0].y = 1.0f;
	draw_GS_PerFrameBufferVariables.quadTexC[1].x = 1.0f;
	draw_GS_PerFrameBufferVariables.quadTexC[1].y = 1.0f;
	draw_GS_PerFrameBufferVariables.quadTexC[2].x = 0.0f;
	draw_GS_PerFrameBufferVariables.quadTexC[2].y = 0.0f;
	draw_GS_PerFrameBufferVariables.quadTexC[3].x = 1.0f;
	draw_GS_PerFrameBufferVariables.quadTexC[3].y = 0.0f;
	mBufferCache.drawGSPerFrameBuffer.quadTexC[0] = draw_GS_PerFrameBufferVariables.quadTexC[0];
	mBufferCache.drawGSPerFrameBuffer.quadTexC[1] = draw_GS_PerFrameBufferVariables.quadTexC[1];
	mBufferCache.drawGSPerFrameBuffer.quadTexC[2] = draw_GS_PerFrameBufferVariables.quadTexC[2];
	mBufferCache.drawGSPerFrameBuffer.quadTexC[3] = draw_GS_PerFrameBufferVariables.quadTexC[3];

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(DRAW_GS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &draw_GS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &draw_GS_PerFrameBuffer);

	// STREAMOUT GS PER FRAME
	ZeroMemory(&streamOut_GS_perFrameBufferVariables, sizeof(STREAMOUT_GS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc_2;
	cbDesc_2.ByteWidth = sizeof(STREAMOUT_GS_PERFRAMEBUFFER);
	cbDesc_2.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc_2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc_2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc_2.MiscFlags = 0;
	cbDesc_2.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData_2;
	InitData_2.pSysMem = &streamOut_GS_perFrameBufferVariables;
	InitData_2.SysMemPitch = 0;
	InitData_2.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc_2, &InitData_2, &streamOut_GS_PerFrameBuffer);

	//------------------------
	// Vertex shader buffers
	//------------------------
	// DRAW VS INIT BUFFER
	ZeroMemory(&draw_VS_InitBufferVariables, sizeof(DRAW_VS_INITBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc_3;
	cbDesc_3.ByteWidth = sizeof(DRAW_VS_INITBUFFER);
	cbDesc_3.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc_3.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc_3.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc_3.MiscFlags = 0;
	cbDesc_3.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData_3;
	InitData_3.pSysMem = &draw_VS_InitBufferVariables;
	InitData_3.SysMemPitch = 0;
	InitData_3.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc_3, &InitData_3, &draw_VS_InitBuffer);

	mInputLayout = inputLayout;

	return true;
}

/*
bool ParticleSystemShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}
*/

bool ParticleSystemShader::BindShaders(
	ID3D11VertexShader* streamOutVS,
	ID3D11GeometryShader* streamOutGS,
	ID3D11VertexShader* drawVS,
	ID3D11GeometryShader* drawGS,
	ID3D11PixelShader* drawPS)
{
	mStreamOutVS = streamOutVS;
	mStreamOutGS = streamOutGS;
	mDrawParticleVS = drawVS;
	mDrawParticleGS = drawGS;
	mDrawParticlePS = drawPS;

	return true;
}

/*
bool ParticleSystemShader::SetActive(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	return true;
}
*/

void ParticleSystemShader::SetViewProj(XMMATRIX& viewProj, XMMATRIX& view)
{
	mBufferCache.drawGSPerFrameBuffer.viewProj = XMMatrixTranspose(viewProj);
	mBufferCache.drawGSPerFrameBuffer.view = XMMatrixTranspose(view);
}

void ParticleSystemShader::SetTexArray(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* texArray)
{
	mTexArray = texArray;
}

void ParticleSystemShader::SetRandomTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* randomTex)
{
	mRandomTex = randomTex;
}

void ParticleSystemShader::SetEyePosW(XMFLOAT3 eyePosW)
{
	mBufferCache.drawGSPerFrameBuffer.eyePosW = eyePosW;
	mBufferCache.streamOutGSPerFrameBuffer.eyePosW = eyePosW;
}

void ParticleSystemShader::SetEmitProperties(XMFLOAT3 emitPosW, XMFLOAT3 emitDirW)
{
	mBufferCache.streamOutGSPerFrameBuffer.emitPosW = emitPosW;
	mBufferCache.streamOutGSPerFrameBuffer.emitDirW = emitDirW;
}

void ParticleSystemShader::SetTime(float gameTime, float dt)
{
	mBufferCache.streamOutGSPerFrameBuffer.gameTime = gameTime;
	mBufferCache.streamOutGSPerFrameBuffer.timeStep = dt;

	mBufferCache.drawVSInitBuffer.timeStep = dt;
}

void ParticleSystemShader::UpdateDrawShaders(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Geometry shader per frame buffer
	dc->Map(draw_GS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	DRAW_GS_PERFRAMEBUFFER* dataPtr = (DRAW_GS_PERFRAMEBUFFER*)mappedResource.pData;
	*dataPtr = mBufferCache.drawGSPerFrameBuffer;

	dc->Unmap(draw_GS_PerFrameBuffer, 0);

	dc->GSSetConstantBuffers(0, 1, &draw_GS_PerFrameBuffer);

	// Vertex shader init buffer
	dc->Map(draw_VS_InitBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	DRAW_VS_INITBUFFER* dataPtr_2 = (DRAW_VS_INITBUFFER*)mappedResource.pData;
	*dataPtr_2 = mBufferCache.drawVSInitBuffer;

	dc->Unmap(draw_VS_InitBuffer, 0);

	dc->VSGetConstantBuffers(0, 1, &draw_VS_InitBuffer);

	dc->PSSetShaderResources(0, 1, &mTexArray);
	dc->PSSetShaderResources(1, 1, &mLitSceneTex);
	dc->PSSetShaderResources(2, 1, &mDepthTex);
}

void ParticleSystemShader::UpdateStreamOutShaders(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(streamOut_GS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	STREAMOUT_GS_PERFRAMEBUFFER* dataPtr = (STREAMOUT_GS_PERFRAMEBUFFER*)mappedResource.pData;
	*dataPtr = mBufferCache.streamOutGSPerFrameBuffer;

	dc->Unmap(streamOut_GS_PerFrameBuffer, 0);

	dc->GSSetConstantBuffers(0, 1, &streamOut_GS_PerFrameBuffer);

	// Set textures
	dc->GSSetShaderResources(0, 1, &mRandomTex);
}

void ParticleSystemShader::ActivateDrawShaders(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mDrawParticleVS, nullptr, 0);
	dc->GSSetShader(mDrawParticleGS, nullptr, 0);
	dc->PSSetShader(mDrawParticlePS, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mPointClampedSS);
}

void ParticleSystemShader::ActivateStreamShaders(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mStreamOutVS, nullptr, 0);
	dc->GSSetShader(mStreamOutGS, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);

	dc->GSSetSamplers(0, 1, &RenderStates::mLinearSS);
}

void ParticleSystemShader::SetAccelConstant(XMFLOAT3 accelConstant)
{
	mBufferCache.drawVSInitBuffer.accelW = accelConstant;
}

void ParticleSystemShader::SetParticleProperties(float particleAgeLimit, float emitFrequency)
{
	mBufferCache.streamOutGSPerFrameBuffer.particleAgeLimit = particleAgeLimit;
	mBufferCache.streamOutGSPerFrameBuffer.emitFrequency = emitFrequency;
}

void ParticleSystemShader::SetTextureIndex(UINT textureIndex)
{
	mBufferCache.drawGSPerFrameBuffer.textureIndex = textureIndex;
}

void ParticleSystemShader::SetParticleType(UINT particleType)
{
	mBufferCache.streamOutGSPerFrameBuffer.particleType = particleType;
}

void ParticleSystemShader::SetLitSceneTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* litSceneTex)
{
	mLitSceneTex = litSceneTex;
}

void ParticleSystemShader::SetPrevViewProj(XMMATRIX& prevViewProj)
{
	mBufferCache.drawGSPerFrameBuffer.prevViewProj = XMMatrixTranspose(prevViewProj);
}

void ParticleSystemShader::SetEmitParticles(bool emitParticles)
{
	mBufferCache.streamOutGSPerFrameBuffer.emitParticles = (emitParticles? 1 : 0);
}

void ParticleSystemShader::SetParticleFadeTime(float fadeTime)
{
	mBufferCache.drawVSInitBuffer.fadeTime = fadeTime;
}

void ParticleSystemShader::SetBlendingMethod(UINT blendingMethod)
{
	mBufferCache.drawGSPerFrameBuffer.blendingMethod = blendingMethod;
}

void ParticleSystemShader::SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* depthTex)
{
	mDepthTex = depthTex;
}

void ParticleSystemShader::SetFarNearClipDistance(float zFar, float zNear)
{
	mBufferCache.drawGSPerFrameBuffer.farClipDistance = zFar;
	mBufferCache.drawGSPerFrameBuffer.nearClipDistance = zNear;
}

void ParticleSystemShader::SetScale(float scaleX, float scaleY)
{
	mBufferCache.streamOutGSPerFrameBuffer.scaleX = scaleX;
	mBufferCache.streamOutGSPerFrameBuffer.scaleY = scaleY;
}

BasicDeferredSkinnedSortedShader::BasicDeferredSkinnedSortedShader()
{

}

BasicDeferredSkinnedSortedShader::~BasicDeferredSkinnedSortedShader()
{
	if (vs_cPerObjBuffer)
		vs_cPerObjBuffer->Release();

	if (vs_cSkinnedBuffer)
		vs_cSkinnedBuffer->Release();

	if (ps_cPerObjBuffer)
		ps_cPerObjBuffer->Release();
}

bool BasicDeferredSkinnedSortedShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&vs_cPerObjBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerObjBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerObjBuffer);

	// SKINNED BUFFER
	ZeroMemory(&vs_cSkinnedBufferVariables, sizeof(VS_CSKINNEDBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbSkinDesc;
	cbSkinDesc.ByteWidth = sizeof(VS_CSKINNEDBUFFER);
	cbSkinDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbSkinDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbSkinDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbSkinDesc.MiscFlags = 0;
	cbSkinDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA SkinInitData;
	SkinInitData.pSysMem = &vs_cSkinnedBufferVariables;
	SkinInitData.SysMemPitch = 0;
	SkinInitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbSkinDesc, &SkinInitData, &vs_cSkinnedBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&ps_cPerObjBufferVariables, sizeof(PS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc2;
	cbDesc2.ByteWidth = sizeof(PS_CPEROBJBUFFER);
	cbDesc2.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc2.MiscFlags = 0;
	cbDesc2.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData2;
	InitData2.pSysMem = &ps_cPerObjBufferVariables;
	InitData2.SysMemPitch = 0;
	InitData2.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc2, &InitData2, &ps_cPerObjBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool BasicDeferredSkinnedSortedShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool BasicDeferredSkinnedSortedShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);

	return true;
}

void BasicDeferredSkinnedSortedShader::SetWorldViewProjTex(XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	mBufferCache.vsPerObjBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
	mBufferCache.vsPerObjBuffer.worldInvTranspose = XMMatrixTranspose(MathHelper::InverseTranspose(world));
	mBufferCache.vsPerObjBuffer.texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

void BasicDeferredSkinnedSortedShader::SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj)
{
	mBufferCache.vsPerObjBuffer.prevWorldViewProj = XMMatrixTranspose(XMMatrixMultiply(prevWorld, prevViewProj));
}

void BasicDeferredSkinnedSortedShader::SetMaterial(const Material& mat)
{
	mBufferCache.psPerObjBuffer.mat = mat;
}

void BasicDeferredSkinnedSortedShader::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void BasicDeferredSkinnedSortedShader::SetBoneTransforms(const XMFLOAT4X4 lowerBodyTransforms[], UINT numLowerBodyTransforms, const XMFLOAT4X4 upperBodyTransforms[], UINT numUpperBodyTransforms)
{
	for (UINT i = 0; i < numLowerBodyTransforms; ++i)
	{
		mBufferCache.vsSkinnedBuffer.lowerBodyTransforms[i] = XMLoadFloat4x4(&lowerBodyTransforms[i]);
		mBufferCache.vsSkinnedBuffer.lowerBodyTransforms[i] = XMMatrixTranspose(mBufferCache.vsSkinnedBuffer.lowerBodyTransforms[i]);
	}

	mBufferCache.vsSkinnedBuffer.numLowerBodyBoneTransforms = numLowerBodyTransforms;

	for (UINT i = 0; i < numUpperBodyTransforms; ++i)
	{
		mBufferCache.vsSkinnedBuffer.upperBodyTransforms[i] = XMLoadFloat4x4(&upperBodyTransforms[i]);
		mBufferCache.vsSkinnedBuffer.upperBodyTransforms[i] = XMMatrixTranspose(mBufferCache.vsSkinnedBuffer.upperBodyTransforms[i]);
	}

	mBufferCache.vsSkinnedBuffer.numUpperBodyBoneTransforms = numUpperBodyTransforms;
}

void BasicDeferredSkinnedSortedShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

	*dataPtr = mBufferCache.vsPerObjBuffer;

	dc->Unmap(vs_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);

	// Vertex shader per obj skinned buffer
	dc->Map(vs_cSkinnedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CSKINNEDBUFFER* dataPtr3 = (VS_CSKINNEDBUFFER*)mappedResource.pData;

	*dataPtr3 = mBufferCache.vsSkinnedBuffer;

	dc->Unmap(vs_cSkinnedBuffer, 0);

	dc->VSSetConstantBuffers(1, 1, &vs_cSkinnedBuffer);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	*dataPtr2 = mBufferCache.psPerObjBuffer;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

void BasicDeferredSkinnedSortedShader::SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void BasicDeferredSkinnedSortedShader::SetShadowTransform(XMMATRIX& shadowTransform)
{
	mBufferCache.vsPerObjBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

void BasicDeferredSkinnedSortedShader::SetRootBoneIndex(UINT rootBoneIndex)
{
	mBufferCache.vsSkinnedBuffer.rootBoneIndex = rootBoneIndex;
}

#pragma region SMAA
#pragma region SMAAColorEdgeDetectionShader
SMAAColorEdgeDetectionShader::SMAAColorEdgeDetectionShader()
{

}

SMAAColorEdgeDetectionShader::~SMAAColorEdgeDetectionShader()
{
	if (VS_PerFrameBuffer)
		VS_PerFrameBuffer->Release();
}

bool SMAAColorEdgeDetectionShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	HRESULT hr;

	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&VS_PerFrameBufferVariables, sizeof(VS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, &VS_PerFrameBuffer);

	if (FAILED(hr))
		return false;

	mInputLayout = inputLayout;

	return true;
}

bool SMAAColorEdgeDetectionShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mPointClampedSS);

	return true;
}

void SMAAColorEdgeDetectionShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(VS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_PERFRAMEBUFFER* dataPtr = (VS_PERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr = mBufferCache.VS_PerFrameBuffer;

	dc->Unmap(VS_PerFrameBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &VS_PerFrameBuffer);
}

bool SMAAColorEdgeDetectionShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SMAAColorEdgeDetectionShader::SetScreenDimensions(UINT screenHeight, UINT screenWidth)
{
	mBufferCache.VS_PerFrameBuffer.screenHeight = screenHeight;
	mBufferCache.VS_PerFrameBuffer.screenWidth = screenWidth;
}

void SMAAColorEdgeDetectionShader::SetColorTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void SMAAColorEdgeDetectionShader::SetPredicationTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}
#pragma endregion SMAAColorEdgeDetectionShader

#pragma region SMAADepthEdgeDetectionShader
SMAADepthEdgeDetectionShader::SMAADepthEdgeDetectionShader()
{

}

SMAADepthEdgeDetectionShader::~SMAADepthEdgeDetectionShader()
{
	if (VS_PerFrameBuffer)
		VS_PerFrameBuffer->Release();

	if (PS_PerFrameBuffer)
		PS_PerFrameBuffer->Release();
}

bool SMAADepthEdgeDetectionShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	HRESULT hr;

	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&VS_PerFrameBufferVariables, sizeof(VS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, &VS_PerFrameBuffer);

	if (FAILED(hr))
		return false;

	//------------------------
	// Pixel shader buffer
	//------------------------
	ZeroMemory(&PS_PerFrameBufferVariables, sizeof(PS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	//D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(PS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	//D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &PS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, &PS_PerFrameBuffer);

	if (FAILED(hr))
		return false;

	mInputLayout = inputLayout;

	return true;
}

bool SMAADepthEdgeDetectionShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearClampedSS);

	return true;
}

void SMAADepthEdgeDetectionShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader frame buffer
	dc->Map(VS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_PERFRAMEBUFFER* dataPtr = (VS_PERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr = mBufferCache.VS_PerFrameBuffer;

	dc->Unmap(VS_PerFrameBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &VS_PerFrameBuffer);

	// Update constant shader buffers using our cache
	//D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Pixel shader framebuffer
	dc->Map(PS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_PERFRAMEBUFFER* dataPtr2 = (PS_PERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr2 = mBufferCache.PS_PerFrameBuffer;

	dc->Unmap(PS_PerFrameBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &PS_PerFrameBuffer);
}

bool SMAADepthEdgeDetectionShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SMAADepthEdgeDetectionShader::SetScreenDimensions(UINT screenHeight, UINT screenWidth)
{
	mBufferCache.VS_PerFrameBuffer.screenWidth = screenWidth;
	mBufferCache.VS_PerFrameBuffer.screenHeight = screenHeight;
	mBufferCache.PS_PerFrameBuffer.screenWidth = screenWidth;
	mBufferCache.PS_PerFrameBuffer.screenHeight = screenHeight;
}

void SMAADepthEdgeDetectionShader::SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}
#pragma endregion SMAADepthEdgeDetectionShader

#pragma region SMAALumaEdgeDetectionShader
SMAALumaEdgeDetectionShader::SMAALumaEdgeDetectionShader()
{

}

SMAALumaEdgeDetectionShader::~SMAALumaEdgeDetectionShader()
{
	if (VS_PerFrameBuffer)
		VS_PerFrameBuffer->Release();
}

bool SMAALumaEdgeDetectionShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	HRESULT hr;

	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&VS_PerFrameBufferVariables, sizeof(VS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, &VS_PerFrameBuffer);

	if (FAILED(hr))
		return false;

	mInputLayout = inputLayout;

	return true;
}

bool SMAALumaEdgeDetectionShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mPointClampedSS);

	return true;
}

void SMAALumaEdgeDetectionShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(VS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_PERFRAMEBUFFER* dataPtr = (VS_PERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr = mBufferCache.VS_PerFrameBuffer;

	dc->Unmap(VS_PerFrameBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &VS_PerFrameBuffer);
}

bool SMAALumaEdgeDetectionShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SMAALumaEdgeDetectionShader::SetScreenDimensions(UINT screenHeight, UINT screenWidth)
{
	mBufferCache.VS_PerFrameBuffer.screenWidth = screenWidth;
	mBufferCache.VS_PerFrameBuffer.screenHeight = screenHeight;
}

void SMAALumaEdgeDetectionShader::SetColorTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void SMAALumaEdgeDetectionShader::SetPredicationTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}
#pragma endregion SMAALumaEdgeDetectionShader

#pragma region SMAABlendingWeightCalculationsShader
SMAABlendingWeightCalculationsShader::SMAABlendingWeightCalculationsShader()
{

}

SMAABlendingWeightCalculationsShader::~SMAABlendingWeightCalculationsShader()
{
	if (VS_PerFrameBuffer)
		VS_PerFrameBuffer->Release();

	if (PS_PerFrameBuffer)
		PS_PerFrameBuffer->Release();
}

bool SMAABlendingWeightCalculationsShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	HRESULT hr;

	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&VS_PerFrameBufferVariables, sizeof(VS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, &PS_PerFrameBuffer);

	if (FAILED(hr))
		return false;

	//------------------------
	// Pixel shader buffer
	//------------------------
	ZeroMemory(&PS_PerFrameBufferVariables, sizeof(PS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	//D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(PS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	//D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &PS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, &VS_PerFrameBuffer);
	
	if (FAILED(hr))
		return false;

	mInputLayout = inputLayout;

	return true;
}

bool SMAABlendingWeightCalculationsShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);
	
	dc->PSSetSamplers(0, 1, &RenderStates::mLinearClampedSS);

	return true;
}

void SMAABlendingWeightCalculationsShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(VS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_PERFRAMEBUFFER* dataPtr = (VS_PERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr = mBufferCache.VS_PerFrameBuffer;

	dc->Unmap(VS_PerFrameBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &VS_PerFrameBuffer);

	// Update constant shader buffers using our cache
	//D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Pixel shader framebuffer
	dc->Map(PS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_PERFRAMEBUFFER* dataPtr2 = (PS_PERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr2 = mBufferCache.PS_PerFrameBuffer;

	dc->Unmap(PS_PerFrameBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &PS_PerFrameBuffer);
}

bool SMAABlendingWeightCalculationsShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SMAABlendingWeightCalculationsShader::SetScreenDimensions(UINT screenHeight, UINT screenWidth)
{
	mBufferCache.VS_PerFrameBuffer.screenWidth = screenWidth;
	mBufferCache.VS_PerFrameBuffer.screenHeight = screenHeight;
	mBufferCache.PS_PerFrameBuffer.screenWidth = screenWidth;
	mBufferCache.PS_PerFrameBuffer.screenHeight = screenHeight;
}

void SMAABlendingWeightCalculationsShader::SetEdgesTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void SMAABlendingWeightCalculationsShader::SetAreaTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void SMAABlendingWeightCalculationsShader::SetSearchTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(2, 1, &tex);
}
#pragma endregion SMAABlendingWeightCalculationsShader

#pragma region SMAANeighborhoodBlendingShader
SMAANeighborhoodBlendingShader::SMAANeighborhoodBlendingShader()
{

}

SMAANeighborhoodBlendingShader::~SMAANeighborhoodBlendingShader()
{
	if (VS_PerFrameBuffer)
		VS_PerFrameBuffer->Release();

	if (PS_PerFrameBuffer)
		PS_PerFrameBuffer->Release();
}

bool SMAANeighborhoodBlendingShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	HRESULT hr;

	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&VS_PerFrameBufferVariables, sizeof(VS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, &VS_PerFrameBuffer);

	if (FAILED(hr))
		return false;

	//------------------------
	// Pixel shader buffer
	//------------------------
	ZeroMemory(&PS_PerFrameBufferVariables, sizeof(PS_PERFRAMEBUFFER));

	// Fill in a buffer description.
	//D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(PS_PERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	//D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &PS_PerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, &PS_PerFrameBuffer);

	if (FAILED(hr))
		return false;

	mInputLayout = inputLayout;

	return true;
}

bool SMAANeighborhoodBlendingShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearClampedSS);

	return true;
}

void SMAANeighborhoodBlendingShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(VS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_PERFRAMEBUFFER* dataPtr = (VS_PERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr = mBufferCache.VS_PerFrameBuffer;

	dc->Unmap(VS_PerFrameBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &VS_PerFrameBuffer);

	// Update constant shader buffers using our cache
	//D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Pixel shader framebuffer
	dc->Map(PS_PerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_PERFRAMEBUFFER* dataPtr2 = (PS_PERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr2 = mBufferCache.PS_PerFrameBuffer;

	dc->Unmap(PS_PerFrameBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &PS_PerFrameBuffer);
}

bool SMAANeighborhoodBlendingShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SMAANeighborhoodBlendingShader::SetScreenDimensions(UINT screenHeight, UINT screenWidth)
{
	mBufferCache.VS_PerFrameBuffer.screenWidth = screenWidth;
	mBufferCache.VS_PerFrameBuffer.screenHeight = screenHeight;
	mBufferCache.PS_PerFrameBuffer.screenWidth = screenWidth;
	mBufferCache.PS_PerFrameBuffer.screenHeight = screenHeight;
}

void SMAANeighborhoodBlendingShader::SetColorTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void SMAANeighborhoodBlendingShader::SetBlendTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void SMAANeighborhoodBlendingShader::SetVelocityTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(2, 1, &tex);
}
#pragma endregion SMAANeighborhoodBlendingShader
#pragma endregion SMAA