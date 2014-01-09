#include "ShaderHandler.h"

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
	mSSAOShader = new SSAOShader();
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

	delete mBasicShader;
	delete mSkyShader;
	delete mNormalSkinned;
	delete mShadowShader;
	delete mBasicDeferredShader;
	delete mBasicDeferredSkinnedShader;
	delete mLightDeferredShader;
	delete mSSAOShader;
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
		return NULL;
}

ID3D11PixelShader* ShaderHandler::GetPixelShader(std::string name)
{
	if (mPixelShaders[name])
		return mPixelShaders[name];
	else
		return NULL;
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

void BasicShader::SetShadowTransform(ID3D11DeviceContext* dc, const XMFLOAT4X4& shadowTransform)
{
	XMMATRIX sTransform = XMLoadFloat4x4(&shadowTransform);
	mBufferCache.vsPerObjBuffer.shadowTransform = sTransform;
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
	if (mVertexShader)
		mVertexShader->Release();
	if (mPixelShader)
		mPixelShader->Release();

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
	
	//test
	//D3D11_DEPTH_STENCIL_DESC dsDesc;
	//dsDesc.DepthEnable = true;
	//dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	//device->CreateDepthStencilState(&dsDesc, &DSState);

	mInputLayout = inputLayout;

	return true;
}

bool ShadowShader::SetActive(ID3D11DeviceContext* dc)
{
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(NULL, nullptr, 0);
	
	//test
	//dc->OMSetDepthStencilState(DSState, 0);

	return true;
}

void ShadowShader::setLightWVP(ID3D11DeviceContext* dc, const XMMATRIX& lgwp)
{
	mBufferCache.vsBuffer.lightWorldViewProj = lgwp;
}

void ShadowShader::updatePerObj(ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dc->Map(vs_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr->lightWorldViewProj = mBufferCache.vsBuffer.lightWorldViewProj;

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

	dataPtr->WorldViewProj = mBufferCache.vsBuffer.WorldViewProj;

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

#pragma endregion NormalMapSkinned

BasicDeferredShader::BasicDeferredShader()
{

}

BasicDeferredShader::~BasicDeferredShader()
{
	if (vs_cPerObjBuffer)
		vs_cPerObjBuffer->Release();
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

	return true;
}

void BasicDeferredShader::SetWorldViewProjTex(XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	mBufferCache.vsPerObjBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
	mBufferCache.vsPerObjBuffer.worldInvTranspose = MathHelper::InverseTranspose(world);
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

	dataPtr->world = mBufferCache.vsPerObjBuffer.world;
	dataPtr->worldViewProj = mBufferCache.vsPerObjBuffer.worldViewProj;
	//dataPtr->worldViewProjTex = mBufferCache.vsBuffer.worldViewProjTex;
	dataPtr->worldInvTranspose = mBufferCache.vsPerObjBuffer.worldInvTranspose;
	dataPtr->texTransform = mBufferCache.vsPerObjBuffer.texTransform;

	dc->Unmap(vs_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr2->mat = mBufferCache.psPerObjBuffer.mat;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

BasicDeferredSkinnedShader::BasicDeferredSkinnedShader()
{

}

BasicDeferredSkinnedShader::~BasicDeferredSkinnedShader()
{

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

	return true;
}

void BasicDeferredSkinnedShader::SetWorldViewProjTex(XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	mBufferCache.vsPerObjBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
	mBufferCache.vsPerObjBuffer.worldInvTranspose = MathHelper::InverseTranspose(world);
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

	dataPtr->world = mBufferCache.vsPerObjBuffer.world;
	dataPtr->worldViewProj = mBufferCache.vsPerObjBuffer.worldViewProj;
	//dataPtr->worldViewProjTex = mBufferCache.vsBuffer.worldViewProjTex;
	dataPtr->worldInvTranspose = mBufferCache.vsPerObjBuffer.worldInvTranspose;
	dataPtr->texTransform = mBufferCache.vsPerObjBuffer.texTransform;

	dc->Unmap(vs_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);

	// Vertex shader per obj skinned buffer
	dc->Map(vs_cSkinnedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CSKINNEDBUFFER* dataPtr3 = (VS_CSKINNEDBUFFER*)mappedResource.pData;

	for (UINT i = 0; i < mBufferCache.vsSkinnedBuffer.numBoneTransforms; ++i)
		dataPtr3->boneTransforms[i] = mBufferCache.vsSkinnedBuffer.boneTransforms[i];

	dataPtr3->numBoneTransforms = mBufferCache.vsSkinnedBuffer.numBoneTransforms;
	dataPtr3->padding = 0;
	dataPtr3->padding2 = 0;
	dataPtr3->padding3 = 0;

	dc->Unmap(vs_cSkinnedBuffer, 0);

	dc->VSSetConstantBuffers(1, 1, &vs_cSkinnedBuffer);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr2->mat = mBufferCache.psPerObjBuffer.mat;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

void BasicDeferredSkinnedShader::SetMaterial(const Material& mat)
{
	mBufferCache.psPerObjBuffer.mat = mat;
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

void LightDeferredShader::SetPositionTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(3, 1, &tex);
}

void LightDeferredShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr->worldViewProj = mBufferCache.vsPerObjBuffer.worldViewProj;

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

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(PS_CPERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &ps_cPerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &ps_cPerFrameBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool SSAOShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);

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

void SSAOShader::SetEyePos(const XMFLOAT3 &eyePos)
{
	ps_cPerFrameBufferVariables.eyePos = eyePos;
}

void SSAOShader::SetZFar(float z_far)
{
	ps_cPerFrameBufferVariables.z_far = z_far;
}

void SSAOShader::SetFramebufferSize(const XMFLOAT2 &framebufferSize)
{
	ps_cPerFrameBufferVariables.framebufferSize = framebufferSize;
}

void SSAOShader::SetProjectionMatrix(const XMMATRIX& projectionMatrix)
{
	ps_cPerFrameBufferVariables.projectionMatrix = XMMatrixTranspose(projectionMatrix);
}

void SSAOShader::SetViewProjectionMatrix(const XMMATRIX& viewProjectionMatrix)
{
	ps_cPerFrameBufferVariables.viewProjectionMatrix = XMMatrixTranspose(viewProjectionMatrix);
	ps_cPerFrameBufferVariables.inverseViewProjectionMatrix = XMMatrixInverse(nullptr, ps_cPerFrameBufferVariables.viewProjectionMatrix);
}

#pragma endregion SSAOShader
