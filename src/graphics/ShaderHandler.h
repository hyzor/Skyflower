#ifndef SHADERHANDLER_H_
#define SHADERHANDLER_H_

#include <string>
#include <vector>
#include <windows.h>
#include <map>
#include "d3dUtilities.h"
#include <d3dcompiler.h>
#include "LightDef.h"
//#include "Vertex.h"
#include "RenderStates.h"
#include "MathHelper.h"

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 16
#define MAX_SPOT_LIGHTS 8

using namespace DirectX;

// Shader interface
// This shader object contains pointers to loaded compiled shaders and handles the
// updating of constant buffers etc.
class IShader
{
public:
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

#pragma region BasicShader
class BasicShader : public IShader
{
public:
	BasicShader();
	~BasicShader();

	// Overload new and delete, because this class contains XMMATRIX (16 byte alignment)
	void* operator new (size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete (void* p)
	{
		BasicShader* ptr = static_cast<BasicShader*>(p);
		_aligned_free(p);
	}

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

	void SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight pointLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirectionalLight dirLights[]);
	void SetSpotLights(ID3D11DeviceContext* dc, UINT numSpotLights, SpotLight spotLights[]);

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
	};

	struct PS_CPEROBJBUFFER
	{
		Material mat;
	};

	struct PS_CPERFRAMEBUFFER
	{
		PointLight pointLights[MAX_POINT_LIGHTS];

		// 16 bytes
		UINT numPointLights;
		int padding2, padding3, padding4;

		DirectionalLight dirLights[MAX_DIR_LIGHTS];

		// 16 bytes
		UINT numDirLights;
		int padding5, padding6, padding7;

		SpotLight spotLights[MAX_SPOT_LIGHTS];

		// 16 bytes
		UINT numSpotLights;
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

	// Override new and delete, because this class contains XMMATRIX (16 byte alignment)
	void* operator new (size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete (void* p)
	{
		BasicShader* ptr = static_cast<BasicShader*>(p);
		_aligned_free(p);
	}

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

#pragma region NormalMappedSkinned
class NormalMappedSkinned : public IShader
{
public:
	NormalMappedSkinned();
	~NormalMappedSkinned();

	// Override new and delete, because this class contains XMMATRIX (16 byte alignment)
	void* operator new (size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete (void* p)
	{
		NormalMappedSkinned* ptr = static_cast<NormalMappedSkinned*>(p);
		_aligned_free(p);
	}

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
	void SetNormalMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight pointLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirectionalLight dirLights[]);
	void SetSpotLights(ID3D11DeviceContext* dc, UINT numSpotLights, SpotLight spotLights[]);

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
		PointLight pointLights[MAX_POINT_LIGHTS];

		// 16 bytes
		UINT numPointLights;
		int padding2, padding3, padding4;

		DirectionalLight dirLights[MAX_DIR_LIGHTS];

		// 16 bytes
		UINT numDirLights;
		int padding5, padding6, padding7;

		SpotLight spotLights[MAX_SPOT_LIGHTS];

		// 16 bytes
		UINT numSpotLights;
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

#pragma region BasicDeferredShader
class BasicDeferredShader : public IShader
{
public:
	BasicDeferredShader();
	~BasicDeferredShader();

	// Overload new and delete, because this class contains XMMATRIX (16 byte alignment)
	void* operator new (size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete (void* p)
	{
		BasicDeferredShader* ptr = static_cast<BasicDeferredShader*>(p);
		_aligned_free(p);
	}

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetMaterial(const Material& mat);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

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
	};

	struct PS_CPEROBJBUFFER
	{
		Material mat;
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

	// Overload new and delete, because this class contains XMMATRIX (16 byte alignment)
	void* operator new (size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete (void* p)
	{
		BasicDeferredSkinnedShader* ptr = static_cast<BasicDeferredSkinnedShader*>(p);
		_aligned_free(p);
	}

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetMaterial(const Material& mat);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetBoneTransforms(const XMFLOAT4X4 boneTransforms[], UINT numTransforms);

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

#pragma region LightDeferredShader
class LightDeferredShader : public IShader
{
public:
	LightDeferredShader();
	~LightDeferredShader();

	// Overload new and delete, because this class contains XMMATRIX (16 byte alignment)
	void* operator new (size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete (void* p)
	{
		LightDeferredShader* ptr = static_cast<LightDeferredShader*>(p);
		_aligned_free(p);
	}

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProj(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj);
	void SetEyePosW(XMFLOAT3 eyePosW);

	void SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight pointLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirectionalLight dirLights[]);
	void SetSpotLights(ID3D11DeviceContext* dc, UINT numSpotLights, SpotLight spotLights[]);

	void SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetSpecularTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetPositionTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void UpdatePerObj(ID3D11DeviceContext* dc);
	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX worldViewProj;
	};

	struct PS_CPERFRAMEBUFFER
	{
		PointLight pointLights[MAX_POINT_LIGHTS];

		// 16 bytes
		UINT numPointLights;
		int padding2, padding3, padding4;

		DirectionalLight dirLights[MAX_DIR_LIGHTS];

		// 16 bytes
		UINT numDirLights;
		int padding5, padding6, padding7;

		SpotLight spotLights[MAX_SPOT_LIGHTS];

		// 16 bytes
		UINT numSpotLights;
		int padding8, padding9, padding10;

		// Forms into a 4D vector
		XMFLOAT3 gEyePosW;
		float padding;
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

#pragma region ShaderHandler
enum ShaderType
{
	VERTEXSHADER = 0,
	PIXELSHADER
};

struct Shader
{
	std::string Name;
	ID3DBlob* Buffer;
	UINT Type;
};

// Loads compiled shader objects
class ShaderHandler
{
public:
	ShaderHandler();
	~ShaderHandler();

	void LoadCompiledVertexShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledPixelShader(LPCWSTR fileName, char* name, ID3D11Device* device);

	ID3D11VertexShader* GetVertexShader(std::string name);
	ID3D11PixelShader* GetPixelShader(std::string name);

	Shader* GetShader(std::string name);

	//void InitShaders(ID3D11Device* device, InputLayouts* inputLayouts);

	BasicShader* mBasicShader;
	SkyShader* mSkyShader;
	NormalMappedSkinned* mNormalSkinned;
	BasicDeferredShader* mBasicDeferredShader;
	BasicDeferredSkinnedShader* mBasicDeferredSkinnedShader;
	LightDeferredShader* mLightDeferredShader;

private:

	BYTE* LoadByteCode(char* fileName, UINT& size);

	//std::vector<ID3D11VertexShader*> mVShaders;
	//std::vector<ID3D11PixelShader*> mPShaders;

	std::map<std::string, ID3D11VertexShader*> mVertexShaders;
	std::map<std::string, ID3D11PixelShader*> mPixelShaders;

	std::vector<Shader*> mShaders;
};
#pragma endregion ShaderHandler

#endif