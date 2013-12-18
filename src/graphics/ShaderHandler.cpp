#include "ShaderHandler.h"

ShaderHandler::ShaderHandler()
{
	// Basic shader
	mBasicShader = new BasicShader();
	mSkyShader = new SkyShader();
	mNormalSkinned = new NormalMappedSkinned();
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
			//SafeDelete(it->second);
			it->second->Release();
	}
	mVertexShaders.clear();

	for (auto& it(mPixelShaders.begin()); it != mPixelShaders.end(); ++it)
	{
		if (it->second)
			//SafeDelete(it->second);
			it->second->Release();
	}
	mPixelShaders.clear();

	delete mBasicShader;
	delete mSkyShader;
	delete mNormalSkinned;
}

void ShaderHandler::LoadCompiledVertexShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = VERTEXSHADER;
	D3DReadFileToBlob(fileName, &shader->Buffer);
	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11VertexShader* vShader;
	device->CreateVertexShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &vShader);
	//mVShaders.push_back(vShader);
	mVertexShaders[name] = vShader;
}

void ShaderHandler::LoadCompiledPixelShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = PIXELSHADER;
	D3DReadFileToBlob(fileName, &shader->Buffer);
	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11PixelShader* pShader;
	device->CreatePixelShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &pShader);
	//mVShaders.push_back(vShader);
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
		numRead = fread(&shaderCode[numRead], 1, fileSize, file);

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

// void ShaderHandler::InitShaders(ID3D11Device* device, InputLayouts* inputLayouts)
// {
// 	// Basic shader
// 	mBasicShader->Init(device, inputLayouts->Basic32);
// }

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

//----------------------------------------------------------------------------
// Basic shader
//----------------------------------------------------------------------------
#pragma region BasicShader
BasicShader::BasicShader()
{

}

BasicShader::~BasicShader()
{
	if (mVertexShader)
		mVertexShader->Release();
	if (mPixelShader)
		mPixelShader->Release();

	if (vs_cBuffer)
		vs_cBuffer->Release();
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
	// Init vertex shader buffer
// 	vs_cBufferVariables.world = XMMatrixIdentity();
// 	vs_cBufferVariables.worldViewProj = XMMatrixIdentity();
// 	vs_cBufferVariables.worldViewProjTex = XMMatrixIdentity();
// 	vs_cBufferVariables.worldInvTranspose = XMMatrixIdentity();
// 	vs_cBufferVariables.texTransform = XMMatrixIdentity();

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

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER OBJECT BUFFER
// 	ps_cPerObjBufferVariables.world = XMMatrixIdentity();
// 	ps_cPerObjBufferVariables.worldViewProj = XMMatrixIdentity();
// 	ps_cPerObjBufferVariables.worldViewProjTex = XMMatrixIdentity();
// 	ps_cPerObjBufferVariables.worldInvTranspose = XMMatrixIdentity();
// 	ps_cPerObjBufferVariables.texTransform = XMMatrixIdentity();

// 	ps_cPerObjBufferVariables.mat.Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	ps_cPerObjBufferVariables.mat.Diffuse = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	ps_cPerObjBufferVariables.mat.Reflect = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
// 	ps_cPerObjBufferVariables.mat.Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);

	//ps_cPerObjBufferVariables.gEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//ps_cPerObjBufferVariables.diffuseMap = nullptr;

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
// 	ps_cPerFrameBufferVariables.gEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
// 	ps_cPerFrameBufferVariables.padding = 0.0f;
// 	ps_cPerFrameBufferVariables.padding2 = 0;
// 	ps_cPerFrameBufferVariables.padding3 = 0;

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
	//dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);

	// Set buffers to the new active shaders
// 	dc->VSSetConstantBuffers(0, 1, &vs_cBuffer);
// 	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
// 	dc->PSSetConstantBuffers(1, 1, &ps_cPerFrameBuffer);

	return true;
}

void BasicShader::UpdatePerObj(ID3D11DeviceContext* dc)
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

	// Pixel shader per obj buffer
	//D3D11_MAPPED_SUBRESOURCE mappedResource2;
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	dataPtr2->mat = mBufferCache.psBuffer.mat;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cBuffer);
	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

void BasicShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Pixel shader per frame buffer
	//D3D11_MAPPED_SUBRESOURCE mappedResource3;
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

	dc->Unmap(ps_cPerFrameBuffer, 0);

	dc->PSSetConstantBuffers(1, 1, &ps_cPerFrameBuffer);
}

void BasicShader::SetWorldViewProjTex(ID3D11DeviceContext* dc,
	XMMATRIX& world,
	XMMATRIX& viewProj,
	XMMATRIX& tex)
{
	//world = XMMatrixTranspose(world);
	//view = XMMatrixTranspose(view);
	//proj = XMMatrixTranspose(proj);
	//tex = XMMatrixTranspose(tex);

	XMMATRIX worldViewProj = XMMatrixMultiply(world, viewProj);
	XMMATRIX worldViewProjTex = XMMatrixMultiply(worldViewProj, tex);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	// Update vertex shader variables
// 	D3D11_MAPPED_SUBRESOURCE mappedResource;
// 	dc->Map(vs_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
// 
// 	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;
// 
// 	dataPtr->world = world;
// 	dataPtr->worldViewProj = worldViewProj;
// 	dataPtr->worldViewProjTex = worldViewProjTex;
// 	dataPtr->worldInvTranspose = worldInvTranspose;
// 	dataPtr->texTransform = texTransform;
// 
// 	dc->Unmap(vs_cBuffer, 0);

	//mBufferCache.vsBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsBuffer.world = world;
	mBufferCache.vsBuffer.worldInvTranspose = worldInvTranspose;
	mBufferCache.vsBuffer.worldViewProj = XMMatrixTranspose(worldViewProj);
	//mBufferCache.vsBuffer.worldViewProjTex = worldViewProjTex;
	mBufferCache.vsBuffer.texTransform = texTransform;
}

void BasicShader::SetEyePosW(ID3D11DeviceContext* dc, XMFLOAT3 eyePosW)
{
	// Update pixel shader variables
// 	D3D11_MAPPED_SUBRESOURCE mappedResource;
// 	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
// 
// 	PS_CPERFRAMEBUFFER* dataPtr = (PS_CPERFRAMEBUFFER*)mappedResource.pData;
// 
// 	dataPtr->gEyePosW = eyePosW;
// 
// 	dc->Unmap(ps_cPerFrameBuffer, 0);

	mBufferCache.psPerFrameBuffer.gEyePosW = eyePosW;
}

void BasicShader::SetMaterial(ID3D11DeviceContext* dc, const Material& mat)
{
	// Update pixel shader variables
// 	D3D11_MAPPED_SUBRESOURCE mappedResource;
// 	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
// 
// 	PS_CPEROBJBUFFER* dataPtr = (PS_CPEROBJBUFFER*)mappedResource.pData;
// 
// 	dataPtr->mat = mat;
// 
// 	dc->Unmap(ps_cPerObjBuffer, 0);

	mBufferCache.psBuffer.mat = mat;
}

void BasicShader::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{	
	// Update pixel shader variables
// 	D3D11_MAPPED_SUBRESOURCE mappedResource;
// 	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
// 
// 	PS_CBUFFER* dataPtr = (PS_CBUFFER*)mappedResource.pData;
// 
// 	dataPtr->diffuseMap = tex;
// 
// 	dc->Unmap(ps_cPerObjBuffer, 0);

	//ps_cPerObjBufferVariables.diffuseMap = tex;

	dc->PSSetShaderResources(0, 1, &tex);
}

bool BasicShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void BasicShader::SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight pointLights[])
{
// 	D3D11_MAPPED_SUBRESOURCE mappedResource;
// 	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
// 
// 	PS_CPERFRAMEBUFFER* dataPtr = (PS_CPERFRAMEBUFFER*)mappedResource.pData;
// 
// 	dataPtr->numPointLights = numPointLights;
// 
// 	for (UINT i = 0; i < numPointLights; ++i)
// 		dataPtr->pointLights[i] = pointLights[i];
// 
// 	dc->Unmap(ps_cPerFrameBuffer, 0);

	mBufferCache.psPerFrameBuffer.numPointLights = numPointLights;

	for (UINT i = 0; i < numPointLights; ++i)
		mBufferCache.psPerFrameBuffer.pointLights[i] = pointLights[i];
}

void BasicShader::SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirectionalLight dirLights[])
{
// 	D3D11_MAPPED_SUBRESOURCE mappedResource;
// 	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
// 
// 	PS_CPERFRAMEBUFFER* dataPtr = (PS_CPERFRAMEBUFFER*)mappedResource.pData;
// 
// 	dataPtr->numDirLights = numDirLights;
// 
// 	for (UINT i = 0; i < numDirLights; ++i)
// 		dataPtr->dirLights[i] = dirLights[i];
// 
// 	dc->Unmap(ps_cPerFrameBuffer, 0);

	mBufferCache.psPerFrameBuffer.numDirLights = numDirLights;

	for (UINT i = 0; i < numDirLights; ++i)
		mBufferCache.psPerFrameBuffer.dirLights[i] = dirLights[i];
}
#pragma endregion BasicShaderEnd

//-----------------------------------------------------------------------------------------------
// Sky shader
//-----------------------------------------------------------------------------------------------
#pragma region SkyShader
SkyShader::SkyShader()
{

}

SkyShader::~SkyShader()
{
	if (mVertexShader)
		mVertexShader->Release();
	if (mPixelShader)
		mPixelShader->Release();

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
	mBufferCache.vsBuffer.WorldViewProj = worldViewProj;
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

	//dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
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

	//dc->PSSetSamplers(0, 1, &samplerState);

	return true;
}
#pragma endregion SkyShaderEnd

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
	//D3D11_MAPPED_SUBRESOURCE mappedResource2;
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
	//D3D11_MAPPED_SUBRESOURCE mappedResource3;
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

	dc->Unmap(ps_cPerFrameBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerFrameBuffer);
}

NormalMappedSkinned::NormalMappedSkinned()
{

}

NormalMappedSkinned::~NormalMappedSkinned()
{
	if (mVertexShader)
		mVertexShader->Release();
	if (mPixelShader)
		mPixelShader->Release();

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
	// PER OBJECT BUFFER
	// 	ps_cPerObjBufferVariables.world = XMMatrixIdentity();
	// 	ps_cPerObjBufferVariables.worldViewProj = XMMatrixIdentity();
	// 	ps_cPerObjBufferVariables.worldViewProjTex = XMMatrixIdentity();
	// 	ps_cPerObjBufferVariables.worldInvTranspose = XMMatrixIdentity();
	// 	ps_cPerObjBufferVariables.texTransform = XMMatrixIdentity();

	// 	ps_cPerObjBufferVariables.mat.Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
	// 	ps_cPerObjBufferVariables.mat.Diffuse = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
	// 	ps_cPerObjBufferVariables.mat.Reflect = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);
	// 	ps_cPerObjBufferVariables.mat.Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 0.25f);

	//ps_cPerObjBufferVariables.gEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//ps_cPerObjBufferVariables.diffuseMap = nullptr;

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
	// 	ps_cPerFrameBufferVariables.gEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	// 	ps_cPerFrameBufferVariables.padding = 0.0f;
	// 	ps_cPerFrameBufferVariables.padding2 = 0;
	// 	ps_cPerFrameBufferVariables.padding3 = 0;

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

	//mBufferCache.vsBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsBuffer.world = world;
	mBufferCache.vsBuffer.worldInvTranspose = worldInvTranspose;
	mBufferCache.vsBuffer.worldViewProj = XMMatrixTranspose(worldViewProj);
	//mBufferCache.vsBuffer.worldViewProjTex = worldViewProjTex;
	mBufferCache.vsBuffer.texTransform = texTransform;
}

#pragma endregion NormalMapSkinnedEnd