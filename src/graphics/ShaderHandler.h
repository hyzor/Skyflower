#ifndef SHADERHANDLER_H_
#define SHADERHANDLER_H_

#include <string>
#include <vector>
//#include <windows.h>
#include <map>
#include "d3dUtilities.h"
#include <d3dcompiler.h>
#include "LightDef.h"
//#include "Vertex.h"
#include "RenderStates.h"
#include "MathHelper.h"

//If you are changing this, change the MAX_POINT_LIGHTS, MAX_DIR_LIGHTS and MAX_SPOT_LIGHTS in LightDef.hlsli
#define MAX_POINT_LIGHTS 16
#define MAX_DIR_LIGHTS 2
#define MAX_SPOT_LIGHTS 32
#define MAX_CASC 4
#define MAX_MATERIALS 64

using namespace DirectX;

static const UINT SHADOWMAP_SR_REG_OFFSET = 1; // Make sure this is the same amount as there are occupied registers before the register for the shadowmaps

// Shader interface
// This shader object contains pointers to loaded compiled shaders and handles the
// updating of constant buffers etc.
class IShader
{
public:
	void* operator new(size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

	virtual bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout) = 0;
	virtual bool SetActive(ID3D11DeviceContext* dc) = 0;

	const ID3D11VertexShader* GetVertexShader() { return mVertexShader; }
	const ID3D11PixelShader* GetPixelShader() { return mPixelShader; }

	virtual void Update(ID3D11DeviceContext* dc) = 0;

protected:
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11InputLayout* mInputLayout;
};

#pragma region ShadowShader
class ShadowShader : public IShader
{
public:
	ShadowShader();
	~ShadowShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool BindVertexShader(ID3D11VertexShader* vShader);

	void setLightWVP(ID3D11DeviceContext* dc, const XMMATRIX& lgwp);
	void updatePerObj(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc){ ; };
	
	struct VS_CPEROBJBUFFER
	{
		XMMATRIX lightWorldViewProj;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	ID3D11Buffer* vs_cBuffer;
	VS_CPEROBJBUFFER vs_cBufferVariables;
};

#pragma endregion ShadowShaderEnd

#pragma region BasicShader
class BasicShader : public IShader
{
public:
	BasicShader();
	~BasicShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(ID3D11DeviceContext* dc,
		XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetEyePosW(ID3D11DeviceContext* dc, XMFLOAT3 eyePosW);
	void SetMaterial(ID3D11DeviceContext* dc, const Material& mat);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* shadowMap);
	void SetShadowTransform(ID3D11DeviceContext* dc, const XMMATRIX& shadowTransform);

	void SetPLights(ID3D11DeviceContext* dc, UINT numPLights, PLight PLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DLight dirLights[]);
	void SetSLights(ID3D11DeviceContext* dc, UINT numSLights, SLight SLights[]);

	void UpdatePerObj(ID3D11DeviceContext* dc);
	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:	
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		//XMMATRIX worldViewProjTex;
		XMMATRIX texTransform;
		XMMATRIX shadowTransform;
	};

	struct PS_CPEROBJBUFFER
	{
		Material mat;
	};

	struct PS_CPERFRAMEBUFFER
	{
		PLight PLights[MAX_POINT_LIGHTS];

		// 16 bytes
		UINT numPLights;
		int padding2, padding3, padding4;

		DLight dirLights[MAX_DIR_LIGHTS];

		// 16 bytes
		UINT numDirLights;
		int padding5, padding6, padding7;

		SLight SLights[MAX_SPOT_LIGHTS];

		// 16 bytes
		UINT numSLights;
		int padding8, padding9, padding10;

		// Forms into a 4D vector
		XMFLOAT3 gEyePosW;
		float padding;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		PS_CPEROBJBUFFER psPerObjBuffer;
		PS_CPERFRAMEBUFFER psPerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// PS - per object
	ID3D11Buffer* ps_cPerObjBuffer;
	PS_CPEROBJBUFFER ps_cPerObjBufferVariables;

	// PS - per frame
	ID3D11Buffer* ps_cPerFrameBuffer;
	PS_CPERFRAMEBUFFER ps_cPerFrameBufferVariables;
};
#pragma endregion BasicShaderEnd

#pragma region SkyShader
class SkyShader : public IShader
{
public:
	SkyShader();
	~SkyShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetWorldViewProj(const XMMATRIX& worldViewProj);
	void SetCubeMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* cubeMap);

	void Update(ID3D11DeviceContext* dc);

private:
	struct VS_CPERFRAMEBUFFER
	{
		XMMATRIX WorldViewProj;
	};

	struct BUFFERCACHE
	{
		VS_CPERFRAMEBUFFER vsBuffer;
	};

	// VS - per frame
	ID3D11Buffer* vs_cPerFrameBuffer;
	VS_CPERFRAMEBUFFER vs_cPerFrameBufferVariables;

	struct BUFFERCACHE mBufferCache;
};
#pragma endregion SkyShader

#pragma region SkyDeferredShader
class SkyDeferredShader : public IShader
{
public:
	SkyDeferredShader();
	~SkyDeferredShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetWorldViewProj(const XMMATRIX& worldViewProj);
	void SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj);
	void SetCubeMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* cubeMap);

	void Update(ID3D11DeviceContext* dc);

private:
	struct VS_CPERFRAMEBUFFER
	{
		XMMATRIX WorldViewProj;
		XMMATRIX prevWorldViewProj;
	};

	struct BUFFERCACHE
	{
		VS_CPERFRAMEBUFFER vsBuffer;
	};

	// VS - per frame
	ID3D11Buffer* vs_cPerFrameBuffer;
	VS_CPERFRAMEBUFFER vs_cPerFrameBufferVariables;

	struct BUFFERCACHE mBufferCache;
};
#pragma endregion SkyDeferredShader

#pragma region NormalMappedSkinned
class NormalMappedSkinned : public IShader
{
public:
	NormalMappedSkinned();
	~NormalMappedSkinned();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(ID3D11DeviceContext* dc,
		XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);
	void SetShadowStransform(ID3D11DeviceContext* dc, XMMATRIX& );

	void SetEyePosW(ID3D11DeviceContext* dc, XMFLOAT3 eyePosW);
	void SetMaterial(ID3D11DeviceContext* dc, const Material& mat);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetNormalMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetPLights(ID3D11DeviceContext* dc, UINT numPLights, PLight PLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DLight dirLights[]);
	void SetSLights(ID3D11DeviceContext* dc, UINT numSLights, SLight SLights[]);

	void SetBoneTransforms(ID3D11DeviceContext* dc, const XMFLOAT4X4 boneTransforms[], UINT numTransforms);

	void UpdatePerObj(ID3D11DeviceContext* dc);
	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		//XMMATRIX worldViewProjTex;
		XMMATRIX texTransform;
		XMMATRIX shadowTransform;
	};

	struct VS_CSKINNEDBUFFER
	{
		XMMATRIX boneTransforms[96];
		UINT numBoneTransforms;
		int padding, padding2, padding3;
	};

	struct PS_CPEROBJBUFFER
	{
		Material mat;
	};

	struct PS_CPERFRAMEBUFFER
	{
		PLight PLights[MAX_POINT_LIGHTS];

		// 16 bytes
		UINT numPLights;
		int padding2, padding3, padding4;

		DLight dirLights[MAX_DIR_LIGHTS];

		// 16 bytes
		UINT numDirLights;
		int padding5, padding6, padding7;

		SLight SLights[MAX_SPOT_LIGHTS];

		// 16 bytes
		UINT numSLights;
		int padding8, padding9, padding10;

		// Forms into a 4D vector
		XMFLOAT3 gEyePosW;
		float padding;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsBuffer;
		VS_CSKINNEDBUFFER vsSkinBuffer;
		PS_CPEROBJBUFFER psBuffer;
		PS_CPERFRAMEBUFFER psPerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cBuffer;
	VS_CPEROBJBUFFER vs_cBufferVariables;

	// VS skinned data
	ID3D11Buffer* vs_cSkinnedBuffer;
	VS_CSKINNEDBUFFER vs_cSkinnedBufferVariables;

	// PS - per object
	ID3D11Buffer* ps_cPerObjBuffer;
	PS_CPEROBJBUFFER ps_cPerObjBufferVariables;

	// PS - per frame
	ID3D11Buffer* ps_cPerFrameBuffer;
	PS_CPERFRAMEBUFFER ps_cPerFrameBufferVariables;
};
#pragma endregion NormalMappedSkinned

#pragma region SkinnedShadowShader
class SkinnedShadowShader : public IShader
{
public:
	SkinnedShadowShader();
	~SkinnedShadowShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool BindVertexShader(ID3D11VertexShader* vShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetLightWVP(ID3D11DeviceContext* dc, XMMATRIX& lwvp);

	void SetBoneTransforms(ID3D11DeviceContext* dc, const XMFLOAT4X4 boneTransforms[], UINT numTransforms);

	void UpdatePerObj(ID3D11DeviceContext* dc);
	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX lightWVP;
	};

	struct VS_CSKINNEDBUFFER
	{
		XMMATRIX boneTransforms[96];
		UINT numBoneTransforms;
		int padding, padding2, padding3;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsBuffer;
		VS_CSKINNEDBUFFER vsSkinBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS
	ID3D11Buffer* vs_cBuffer;
	VS_CPEROBJBUFFER vs_cBufferVariables;

	// VS skinned
	ID3D11Buffer* vs_cSkinnedBuffer;
	VS_CSKINNEDBUFFER vs_cSkinnedBufferVariables;
};
#pragma endregion SkinnedShadowShaderEnd

#pragma region ShadowMorphShader
class ShadowMorphShader : public IShader
{
public:
	ShadowMorphShader();
	~ShadowMorphShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetLightWVP(ID3D11DeviceContext* dc, XMMATRIX& lwvp);

	void SetWeights(XMFLOAT4 weights);

	void UpdatePerObj(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX lightWVP;
		XMFLOAT4 weights;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS
	ID3D11Buffer* vs_cBuffer;
	VS_CPEROBJBUFFER vs_cBufferVariables;
};
#pragma endregion ShadowMorphShaderEnd

#pragma region BasicDeferredSkinnedSortedShadowShader
class BasicDeferredSkinnedSortedShadowShader : public IShader
{
public:
	BasicDeferredSkinnedSortedShadowShader();
	~BasicDeferredSkinnedSortedShadowShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);
	bool BindVertexShader(ID3D11VertexShader* vShader);

	void SetLightWVP(ID3D11DeviceContext* dc, const XMMATRIX& lgwp);
	void SetBoneTransforms(const XMFLOAT4X4 lowerBodyTransforms[], UINT numLowerBodyTransforms, const XMFLOAT4X4 upperBodyTransforms[], UINT numUpperBodyTransforms);
	void SetRootBoneIndex(UINT rootBoneIndex);

	void UpdatePerObj(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX lightWVP;
	};

	struct VS_CSKINNEDBUFFER
	{
		XMMATRIX upperBodyTransforms[64];
		UINT numUpperBodyBoneTransforms;

		UINT rootBoneIndex;
		XMFLOAT2 padding123;

		XMMATRIX lowerBodyTransforms[64];
		UINT numLowerBodyBoneTransforms;
		XMFLOAT3 padding124;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		VS_CSKINNEDBUFFER vsSkinnedBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// VS - skinned data
	ID3D11Buffer* vs_cSkinnedBuffer;
	VS_CSKINNEDBUFFER vs_cSkinnedBufferVariables;
};
#pragma endregion BasicDeferredSkinnedSortedShadowShader

#pragma region BasicDeferredShader
class BasicDeferredShader : public IShader
{
public:
	BasicDeferredShader();
	~BasicDeferredShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj);

	void SetShadowTransformLightViewProj(XMMATRIX& shadowTransform, XMMATRIX& lightView, XMMATRIX& lightProj);

	void SetMaterial(const Material& mat, UINT globalMaterialIndex);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetType(int type);

	void SetCascadeTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex, int index);
	void SetEyeSpaceTransform(const XMMATRIX& view);
	void SetCascadeTransformAndDepths(const XMMATRIX& shadowTransform, float nearDepth, float farDepth, int index);
	void SetNrOfCascades(int nrOfCascades);

	void UpdatePerObj(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		//XMMATRIX worldViewProjTex;
		XMMATRIX texTransform;
		XMMATRIX shadowTransforms[MAX_CASC];
		XMMATRIX prevWorldViewProj;
		XMMATRIX toEyeSpace;
	};
	
	struct PS_CPEROBJBUFFER
	{
		Material mat;

		unsigned int globalMaterialIndex;
		int type;
		XMFLOAT2 padding;

		XMFLOAT4 nearDepths;
		XMFLOAT4 farDepths;
		int nrOfCascades;
		XMFLOAT3 padding1;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		PS_CPEROBJBUFFER psPerObjBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// PS - per object
	ID3D11Buffer* ps_cPerObjBuffer;
	PS_CPEROBJBUFFER ps_cPerObjBufferVariables;
};
#pragma endregion BasicDeferredShader

#pragma region BasicDeferredSkinnedShader
class BasicDeferredSkinnedShader : public IShader
{
public:
	BasicDeferredSkinnedShader();
	~BasicDeferredSkinnedShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj);

	void SetMaterial(const Material& mat, UINT globalMaterialIndex);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetBoneTransforms(const XMFLOAT4X4 boneTransforms[], UINT numTransforms);

	void UpdatePerObj(ID3D11DeviceContext* dc);

	void SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowTransform(XMMATRIX& shadowTransform);

	void SetCascadeTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex, int index);
	void SetEyeSpaceTransform(const XMMATRIX& view);
	void SetCascadeTransformAndDepths(const XMMATRIX& shadowTransform, float nearDepth, float farDepth, int index);
	void SetNrOfCascades(int nrOfCascades);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		//XMMATRIX worldViewProjTex;
		XMMATRIX texTransform;
		XMMATRIX prevWorldViewProj;
		XMMATRIX shadowTransforms[MAX_CASC];
		XMMATRIX toEyeSpace;
	};

	struct VS_CSKINNEDBUFFER
	{
		XMMATRIX boneTransforms[96];
		UINT numBoneTransforms;
		int padding, padding2, padding3;
	};

	struct PS_CPEROBJBUFFER
	{
		Material mat;
		XMFLOAT4 nearDepths;
		XMFLOAT4 farDepths;
		int nrOfCascades;
		unsigned int globalMaterialIndex;
		XMFLOAT2 padding1;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		VS_CSKINNEDBUFFER vsSkinnedBuffer;
		PS_CPEROBJBUFFER psPerObjBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// VS - skinned data
	ID3D11Buffer* vs_cSkinnedBuffer;
	VS_CSKINNEDBUFFER vs_cSkinnedBufferVariables;

	// PS - per obj
	ID3D11Buffer* ps_cPerObjBuffer;
	PS_CPEROBJBUFFER ps_cPerObjBufferVariables;
};
#pragma endregion BasicDeferredSkinnedShader

#pragma region BasicDeferredSkinnedSortedShader
class BasicDeferredSkinnedSortedShader : public IShader
{
public:
	BasicDeferredSkinnedSortedShader();
	~BasicDeferredSkinnedSortedShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj);

	void SetMaterial(const Material& mat, UINT globalMaterialIndex);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetBoneTransforms(const XMFLOAT4X4 lowerBodyTransforms[], UINT numLowerBodyTransforms, const XMFLOAT4X4 upperBodyTransforms[], UINT numUpperBodyTransforms);

	void SetRootBoneIndex(UINT rootBoneIndex);

	void UpdatePerObj(ID3D11DeviceContext* dc);

	void SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowTransform(XMMATRIX& shadowTransform);

	void SetCascadeTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex, int index);
	void SetEyeSpaceTransform(const XMMATRIX& view);
	void SetCascadeTransform(const XMMATRIX& shadowTransform, int index);
	void SetCascadeDepths(float nearDepth, float farDepth, int index);
	void SetNrOfCascades(int nrOfCascades);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		//XMMATRIX worldViewProjTex;
		XMMATRIX texTransform;
		XMMATRIX prevWorldViewProj;
		XMMATRIX shadowTransforms[MAX_CASC];
		XMMATRIX toEyeSpace;
	};

	struct VS_CSKINNEDBUFFER
	{
		XMMATRIX upperBodyTransforms[64];
		UINT numUpperBodyBoneTransforms;

		UINT rootBoneIndex;
		XMFLOAT2 padding123;

		XMMATRIX lowerBodyTransforms[64];
		UINT numLowerBodyBoneTransforms;
		XMFLOAT3 padding124;
	};

	struct PS_CPEROBJBUFFER
	{
		Material mat;
		XMFLOAT4 nearDepths;
		XMFLOAT4 farDepths;
		int nrOfCascades;
		unsigned int globalMaterialIndex;
		XMFLOAT2 padding1;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		VS_CSKINNEDBUFFER vsSkinnedBuffer;
		PS_CPEROBJBUFFER psPerObjBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// VS - skinned data
	ID3D11Buffer* vs_cSkinnedBuffer;
	VS_CSKINNEDBUFFER vs_cSkinnedBufferVariables;

	// PS - per obj
	ID3D11Buffer* ps_cPerObjBuffer;
	PS_CPEROBJBUFFER ps_cPerObjBufferVariables;
};
#pragma endregion BasicDeferredSkinnedSortedShader

#pragma region LightDeferredShader
class LightDeferredShader : public IShader
{
public:
	LightDeferredShader();
	~LightDeferredShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProj(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj);

	void SetEyePosW(XMFLOAT3 eyePosW);
	void SetCameraViewProjMatrix(XMMATRIX& camViewMatrix, XMMATRIX& proj);
	void SetCameraWorldMatrix(XMMATRIX& camWorldMatrix);
	void SetLightWorldViewProj(XMMATRIX& lightWorld, XMMATRIX& lightView, XMMATRIX& lightProj);

	void SetPLights(ID3D11DeviceContext* dc, UINT numPLights, PLight PLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DLight dirLights[]);
	void SetSLights(ID3D11DeviceContext* dc, UINT numSLights, SLight SLights[]);

	void SetMaterials(ID3D11DeviceContext* dc, UINT numMaterials, Material* mats[]);

	void SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetSpecularTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	//void SetPositionTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetSSAOTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetVelocityTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetBackgroundTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowTransform(XMMATRIX& shadowTransform);

	void SetFogProperties(int enableFogging, float heightFalloff, float heightOffset, float globalDensity, XMFLOAT4 fogColor);
	void SetMotionBlurProperties(int enableMotionBlur);
	void SetFpsValues(float curFps, float targetFps);

	void SetSkipLighting(bool skipLighting);
	void SetIsTransparencyPass(bool isTransparencyPass);

	void UpdatePerObj(ID3D11DeviceContext* dc);
	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX worldViewProj;
		XMMATRIX shadowTransform;
		XMMATRIX lightViewProj;
		XMMATRIX viewProjInv;
	};

	struct PS_CPERFRAMEBUFFER
	{
		PLight PLights[MAX_POINT_LIGHTS];
		DLight dirLights[MAX_DIR_LIGHTS];
		SLight SLights[MAX_SPOT_LIGHTS];

		UINT numPLights;
		UINT numDirLights;
		UINT numSLights;
		UINT padding;

		// Forms into a 4D vector
		XMFLOAT3 gEyePosW;
		int isTransparencyPass;

		int enableFogging;
		float fogHeightFalloff, fogHeightOffset, fogGlobalDensity;
		XMFLOAT4 fogColor;

		int enableMotionBlur;
		float curFPS;
		float targetFPS;
		int skipLighting;

		XMMATRIX camViewProjInv;

		Material materials[MAX_MATERIALS];
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		PS_CPERFRAMEBUFFER psPerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// PS - per object
	ID3D11Buffer* ps_cPerFrameBuffer;
	PS_CPERFRAMEBUFFER ps_cPerFrameBufferVariables;
};
#pragma endregion LightDeferredShader

#pragma region SSAOShader
class SSAOShader : public IShader
{
public:
	SSAOShader();
	~SSAOShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void Update(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetRandomTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetInverseProjectionMatrix(const XMMATRIX& inverseProjectionMatrix);
	void SetViewMatrix(const XMMATRIX& viewMatrix);
	void SetZFar(float z_far);

	void SetParameters(float radius, float projection_factor, float bias, float contrast, float sigma);

private:
	struct PS_CPERFRAMEBUFFER
	{
		XMMATRIX inverseProjectionMatrix;
		XMMATRIX viewMatrix;
		float z_far;

		float radius;
		float projection_factor;
		float bias;

		float contrast;
		float sigma;

		float padding[2];
	};

	ID3D11Buffer* ps_cPerFrameBuffer;
	PS_CPERFRAMEBUFFER ps_cPerFrameBufferVariables;
};
#pragma endregion SSAOShader

#pragma region DepthOfFieldCoCShader
class DepthOfFieldCoCShader : public IShader
{
public:
	DepthOfFieldCoCShader();
	~DepthOfFieldCoCShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void Update(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetZNearFar(float z_near, float z_far);
	void SetFocusPlanes(float near_blurry_plane, float near_sharp_plane, float far_sharp_plane, float far_blurry_plane);

private:
	struct PS_CPERFRAMEBUFFER
	{
		float z_near;
		float z_far;
		float near_blurry_plane;
		float near_sharp_plane;
		float far_sharp_plane;
		float far_blurry_plane;
		float near_scale;
		float far_scale;
	};

	ID3D11Buffer* ps_cPerFrameBuffer;
	PS_CPERFRAMEBUFFER ps_cPerFrameBufferVariables;
};
#pragma endregion DepthOfFieldCoCShader

#pragma region BlurShader
class BlurShader : public IShader
{
public:
	BlurShader();
	~BlurShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void Update(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetFramebufferTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetInputTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetFramebufferSize(const XMFLOAT2 &framebufferSize);
	void SetZNearFar(float z_near, float z_far);

private:
	struct PS_CPERFRAMEBUFFER
	{
		XMFLOAT2 framebufferSize;
		float z_near;
		float z_far;
	};

	ID3D11Buffer* ps_cPerFrameBuffer;
	PS_CPERFRAMEBUFFER ps_cPerFrameBufferVariables;
};
#pragma endregion BlurShader

#pragma region BasicDeferredMorphShader
class BasicDeferredMorphShader : public IShader
{
public:
	BasicDeferredMorphShader();
	~BasicDeferredMorphShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj);

	void SetMaterial(const Material& mat, UINT globalMaterialIndex);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void UpdatePerObj(ID3D11DeviceContext* dc);

	void SetWeights(XMFLOAT4 weights);

	void SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowTransform(XMMATRIX& shadowTransform);

	void SetCascadeTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex, int index);
	void SetEyeSpaceTransform(const XMMATRIX& view);
	void SetCascadeTransformAndDepths(const XMMATRIX& shadowTransform, float nearDepth, float farDepth, int index);
	void SetNrOfCascades(int nrOfCascades);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		//XMMATRIX worldViewProjTex;
		XMMATRIX texTransform;
		XMFLOAT4 weights;
		XMMATRIX prevWorldViewProj;
		XMMATRIX shadowTransforms[MAX_CASC];
		XMMATRIX toEyeSpace;
	};

	struct PS_CPEROBJBUFFER
	{
		Material mat;
		XMFLOAT4 nearDepths;
		XMFLOAT4 farDepths;
		int nrOfCascades;
		unsigned int globalMaterialIndex;
		XMFLOAT2 padding1;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		PS_CPEROBJBUFFER psPerObjBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// PS - per obj
	ID3D11Buffer* ps_cPerObjBuffer;
	PS_CPEROBJBUFFER ps_cPerObjBufferVariables;
};
#pragma endregion BasicDeferredMorphShader

#pragma region ParticleSystemShader
class ParticleSystemShader : public IShader
{
public:
	ParticleSystemShader();
	~ParticleSystemShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	//bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	bool BindShaders(
		ID3D11VertexShader* streamOutVS,
		ID3D11GeometryShader* streamOutGS,
		ID3D11VertexShader* drawVS,
		ID3D11GeometryShader* drawGS,
		ID3D11PixelShader* drawPS);

	bool SetActive(ID3D11DeviceContext* dc) { return false; }

	void ActivateDrawShaders(ID3D11DeviceContext* dc);
	void ActivateStreamShaders(ID3D11DeviceContext* dc);

	void SetViewProj(XMMATRIX& viewProj, XMMATRIX& view);
	void SetPrevViewProj(XMMATRIX& prevViewProj);
	void SetTexArray(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* texArray);
	void SetRandomTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* randomTex);

	void SetLitSceneTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* litSceneTex);
	void SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* depthTex);

	void SetFarNearClipDistance(float zFar, float zNear);

	void SetScale(float scaleX, float scaleY);

	void SetEyePosW(XMFLOAT3 eyePosW);
	void SetEmitProperties(XMFLOAT3 emitPosW, XMFLOAT3 emitDirW);
	void SetParticleProperties(float particleAgeLimit, float emitFrequency);

	void SetTime(float gameTime, float dt);

	void SetParticleFadeTime(float fadeTime);

	void SetBlendingMethod(UINT blendingMethod);

	void SetAccelConstant(XMFLOAT3 accelConstant);

	void SetTextureIndex(UINT textureIndex);

	void SetParticleType(UINT particleType);

	void SetEmitParticles(bool emitParticles);

	void SetRandomVelocity(XMFLOAT3 randomVelocity);

	void RandomizeVelocity(bool randomize);

	void SetColor(XMFLOAT3 color);
	//void UpdatePerParticleSystem(ID3D11DeviceContext* dc);

	void UpdateDrawShaders(ID3D11DeviceContext* dc);
	void UpdateStreamOutShaders(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct DRAW_VS_INITBUFFER
	{
		XMFLOAT3 accelW;
		float padding1;

		float fadeTime;
		float timeStep;
		XMFLOAT2 fadeTimePadding;

		XMFLOAT3 color;
		float colorPadding;
	};

	struct DRAW_GS_PERFRAMEBUFFER
	{
		XMFLOAT3 eyePosW;
		float farClipDistance;
		//float padding;

		XMMATRIX view;
		XMMATRIX viewProj;
		XMMATRIX prevViewProj;

		XMFLOAT4 quadTexC[4];

		UINT textureIndex;
		UINT blendingMethod;
		float nearClipDistance;
		float paddingTex;
	};

	struct STREAMOUT_GS_PERFRAMEBUFFER
	{
		XMFLOAT3 eyePosW;
		float gameTime;

		XMFLOAT3 emitPosW;
		float timeStep;

		XMFLOAT3 emitDirW;
		float particleAgeLimit;

		float emitFrequency;
		UINT particleType;

		UINT emitParticles;
		float scaleX;

		float scaleY;
		XMFLOAT3 paddingScale;


		int randomizeVelocity;
		XMFLOAT3 randomVelocity;
	};

	struct BUFFERCACHE
	{
		DRAW_VS_INITBUFFER drawVSInitBuffer;
		DRAW_GS_PERFRAMEBUFFER drawGSPerFrameBuffer;
		STREAMOUT_GS_PERFRAMEBUFFER streamOutGSPerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// Draw VS init buffer
	ID3D11Buffer* draw_VS_InitBuffer;
	DRAW_VS_INITBUFFER draw_VS_InitBufferVariables;

	// Draw GS per frame
	ID3D11Buffer* draw_GS_PerFrameBuffer;
	DRAW_GS_PERFRAMEBUFFER draw_GS_PerFrameBufferVariables;

	// StreamOut GS per frame
	ID3D11Buffer* streamOut_GS_PerFrameBuffer;
	STREAMOUT_GS_PERFRAMEBUFFER streamOut_GS_perFrameBufferVariables;

	// Shaders
	ID3D11VertexShader* mDrawParticleVS;
	ID3D11GeometryShader* mDrawParticleGS;
	ID3D11PixelShader* mDrawParticlePS;

	ID3D11VertexShader* mStreamOutVS;
	ID3D11GeometryShader* mStreamOutGS;

	// Shared data
	ID3D11ShaderResourceView* mTexArray;
	ID3D11ShaderResourceView* mRandomTex;

	ID3D11ShaderResourceView* mLitSceneTex;
	ID3D11ShaderResourceView* mDepthTex;
};
#pragma endregion ParticleSystemShader

#pragma region CompositeShader
class CompositeShader : public IShader
{
public:
	CompositeShader();
	~CompositeShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void Update(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetFramebufferTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetDoFCoCTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetDoFFarFieldTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
};
#pragma endregion CompositeShader

//----------------------------------------------------------------------
// SMAA
//----------------------------------------------------------------------
#pragma region SMAA

#pragma region SMAAColorEdgeDetectionShader
class SMAAColorEdgeDetectionShader : public IShader
{
public:
	SMAAColorEdgeDetectionShader();
	~SMAAColorEdgeDetectionShader();

	struct VS_PERFRAMEBUFFER
	{
		UINT screenWidth;
		UINT screenHeight;
		XMFLOAT2 padding;
	};

	struct BUFFERCACHE
	{
		VS_PERFRAMEBUFFER VS_PerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS per frame buffer
	ID3D11Buffer* VS_PerFrameBuffer;
	VS_PERFRAMEBUFFER VS_PerFrameBufferVariables;

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void UpdatePerFrame(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetScreenDimensions(UINT screenHeight, UINT screenWidth);
	void SetColorTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetPredicationTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

private:
	void Update(ID3D11DeviceContext* dc) { ; }
};
#pragma endregion SMAAColorEdgeDetectionShader

#pragma region SMAADepthEdgeDetectionShader
class SMAADepthEdgeDetectionShader : public IShader
{
public:
	SMAADepthEdgeDetectionShader();
	~SMAADepthEdgeDetectionShader();

	struct VS_PERFRAMEBUFFER
	{
		UINT screenWidth;
		UINT screenHeight;
		XMFLOAT2 padding;
	};

	struct PS_PERFRAMEBUFFER
	{
		UINT screenWidth;
		UINT screenHeight;
		XMFLOAT2 padding;
	};

	struct BUFFERCACHE
	{
		VS_PERFRAMEBUFFER VS_PerFrameBuffer;
		PS_PERFRAMEBUFFER PS_PerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS per frame buffer
	ID3D11Buffer* VS_PerFrameBuffer;
	VS_PERFRAMEBUFFER VS_PerFrameBufferVariables;

	// PS per frame buffer
	ID3D11Buffer* PS_PerFrameBuffer;
	PS_PERFRAMEBUFFER PS_PerFrameBufferVariables;

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void UpdatePerFrame(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetScreenDimensions(UINT screenHeight, UINT screenWidth);
	void SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

private:
	void Update(ID3D11DeviceContext* dc) { ; }
};
#pragma endregion SMAADepthEdgeDetectionShader

#pragma region SMAALumaEdgeDetectionShader
class SMAALumaEdgeDetectionShader : public IShader
{
public:
	SMAALumaEdgeDetectionShader();
	~SMAALumaEdgeDetectionShader();

	struct VS_PERFRAMEBUFFER
	{
		UINT screenWidth;
		UINT screenHeight;
		XMFLOAT2 padding;
	};

	struct BUFFERCACHE
	{
		VS_PERFRAMEBUFFER VS_PerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS per frame buffer
	ID3D11Buffer* VS_PerFrameBuffer;
	VS_PERFRAMEBUFFER VS_PerFrameBufferVariables;

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void UpdatePerFrame(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetScreenDimensions(UINT screenHeight, UINT screenWidth);
	void SetColorTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetPredicationTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

private:
	void Update(ID3D11DeviceContext* dc) { ; }
};
#pragma endregion SMAALumaEdgeDetectionShader

#pragma region SMAABlendingWeightCalculationsShader
class SMAABlendingWeightCalculationsShader : public IShader
{
public:
	SMAABlendingWeightCalculationsShader();
	~SMAABlendingWeightCalculationsShader();

	struct VS_PERFRAMEBUFFER
	{
		UINT screenWidth;
		UINT screenHeight;
		XMFLOAT2 padding;
	};

	struct PS_PERFRAMEBUFFER
	{
		UINT screenWidth;
		UINT screenHeight;
		XMFLOAT2 padding;
	};

	struct BUFFERCACHE
	{
		VS_PERFRAMEBUFFER VS_PerFrameBuffer;
		PS_PERFRAMEBUFFER PS_PerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS per frame buffer
	ID3D11Buffer* VS_PerFrameBuffer;
	VS_PERFRAMEBUFFER VS_PerFrameBufferVariables;

	// PS per frame buffer
	ID3D11Buffer* PS_PerFrameBuffer;
	PS_PERFRAMEBUFFER PS_PerFrameBufferVariables;

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void UpdatePerFrame(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetScreenDimensions(UINT screenHeight, UINT screenWidth);
	void SetEdgesTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetAreaTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetSearchTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

private:
	void Update(ID3D11DeviceContext* dc) { ; }
};
#pragma endregion SMAABlendingWeightCalculationsShader

#pragma region SMAANeighborhoodBlendingShader
class SMAANeighborhoodBlendingShader : public IShader
{
public:
	SMAANeighborhoodBlendingShader();
	~SMAANeighborhoodBlendingShader();

	struct VS_PERFRAMEBUFFER
	{
		UINT screenWidth;
		UINT screenHeight;
		XMFLOAT2 padding;
	};

	struct PS_PERFRAMEBUFFER
	{
		UINT screenWidth;
		UINT screenHeight;
		XMFLOAT2 padding;
	};

	struct BUFFERCACHE
	{
		VS_PERFRAMEBUFFER VS_PerFrameBuffer;
		PS_PERFRAMEBUFFER PS_PerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS per frame buffer
	ID3D11Buffer* VS_PerFrameBuffer;
	VS_PERFRAMEBUFFER VS_PerFrameBufferVariables;

	// PS per frame buffer
	ID3D11Buffer* PS_PerFrameBuffer;
	PS_PERFRAMEBUFFER PS_PerFrameBufferVariables;

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void UpdatePerFrame(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetScreenDimensions(UINT screenHeight, UINT screenWidth);
	void SetColorTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetBlendTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetVelocityTex(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

private:
	void Update(ID3D11DeviceContext* dc) { ; }
};
#pragma endregion SMAANeighborhoodBlendingShader

#pragma endregion SMAA

#pragma region ShaderHandler
enum ShaderType
{
	VERTEXSHADER = 0,
	PIXELSHADER,
	GEOMETRYSHADER,
	GEOMETRYSTREAMOUTSHADER
};

struct Shader
{
	std::string Name;
	ID3DBlob* Buffer;
	UINT Type;
};

#pragma region LineShader
class LineShader : public IShader
{
public:
	LineShader();
	~LineShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	void Update(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetTransformation(const XMFLOAT3X3 &transformation);
	void SetFramebufferSize(const XMFLOAT2 &framebufferSize);
	void SetColor(const XMFLOAT4 &color);

private:
	struct PS_CPEROBJECT
	{
		XMMATRIX transformation;
		XMMATRIX projection;
		XMFLOAT4 color;
	};

	ID3D11Buffer* ps_cPerObjectBuffer;
	PS_CPEROBJECT ps_cPerObjectVariables;
};
#pragma endregion LineShader

// Loads compiled shader objects
class ShaderHandler
{
public:
	ShaderHandler();
	~ShaderHandler();

	void LoadCompiledVertexShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledPixelShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledGeometryShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledGeometryStreamOutShader(LPCWSTR fileName, char* name, ID3D11Device* device, D3D11_SO_DECLARATION_ENTRY pDecl[], UINT pDeclSize, UINT rasterizedStream = 0, UINT* bufferStrides = NULL, UINT numStrides = 0);

	ID3D11VertexShader* GetVertexShader(std::string name);
	ID3D11PixelShader* GetPixelShader(std::string name);
	ID3D11GeometryShader* GetGeometryShader(std::string name);

	Shader* GetShader(std::string name);

	//void InitShaders(ID3D11Device* device, InputLayouts* inputLayouts);

	BasicShader* mBasicShader;
	SkyShader* mSkyShader;
	NormalMappedSkinned* mNormalSkinned;
	ShadowShader* mShadowShader;
	BasicDeferredShader* mBasicDeferredShader;
	BasicDeferredSkinnedShader* mBasicDeferredSkinnedShader;
	LightDeferredShader* mLightDeferredShader;
	SkinnedShadowShader* mSkinnedShadowShader;
	SkyDeferredShader* mSkyDeferredShader;
	SSAOShader* mSSAOShader;
	BlurShader* mSSAOBlurHorizontalShader;
	BlurShader* mSSAOBlurVerticalShader;
	DepthOfFieldCoCShader* mDepthOfFieldCoCShader;
	BlurShader* mDepthOfFieldBlurHorizontalShader;
	BlurShader* mDepthOfFieldBlurVerticalShader;
	CompositeShader* mCompositeShader;
	BasicDeferredMorphShader* mDeferredMorphShader;
	ShadowMorphShader* mShadowMorphShader;
	BasicDeferredSkinnedSortedShadowShader* mSkinnedSortedShadowShader;
	ParticleSystemShader* mParticleSystemShader;
	LightDeferredShader* mLightDeferredToTextureShader;
	BasicDeferredSkinnedSortedShader* mBasicDeferredSkinnedSortedShader;
	LineShader* mLineShader;

	// SMAA
	SMAAColorEdgeDetectionShader* mSMAAColorEdgeDetectionShader;
	SMAALumaEdgeDetectionShader* mSMAALumaEdgeDetectionShader;
	SMAADepthEdgeDetectionShader* mSMAADepthEdgeDetectionShader;
	SMAABlendingWeightCalculationsShader* mSMAABlendingWeightCalculationsShader;
	SMAANeighborhoodBlendingShader* mSMAANeighborhoodBlendingShader;

private:

	BYTE* LoadByteCode(char* fileName, UINT& size);

	//std::vector<ID3D11VertexShader*> mVShaders;
	//std::vector<ID3D11PixelShader*> mPShaders;

	std::map<std::string, ID3D11VertexShader*> mVertexShaders;
	std::map<std::string, ID3D11PixelShader*> mPixelShaders;
	std::map<std::string, ID3D11GeometryShader*> mGeometryShaders;

	std::vector<Shader*> mShaders;
};
#pragma endregion ShaderHandler

#endif