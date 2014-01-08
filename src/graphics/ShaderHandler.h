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

using namespace DirectX;

enum ShaderType
{
	VERTEXSHADER = 0,
	PIXELSHADER
};

// Shader interface
// This shader object contains pointers to loaded compiled shaders and handles the
// updating of constant buffers etc.
class IShader
{
public:
	//IShader();
	//virtual ~IShader();

	virtual bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout) = 0;
	virtual bool SetActive(ID3D11DeviceContext* dc) = 0;
	//virtual void UpdateCBuffers() = 0;

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

	void* operator new (size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete (void* p)
	{
		ShadowShader* ptr = static_cast<ShadowShader*>(p);
		_aligned_free(p);
	}

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
	//ID3D11DepthStencilState* DSState;
	VS_CPEROBJBUFFER vs_cBufferVariables;
};

#pragma endregion ShadowShaderEnd

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
	//bool SetActive(ID3D11DeviceContext* dc) { return false; }
	bool SetActive(ID3D11DeviceContext* dc);

	//void SetWorldViewProj(XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 projection);
	void SetWorldViewProjTex(ID3D11DeviceContext* dc,
		XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetEyePosW(ID3D11DeviceContext* dc, XMFLOAT3 eyePosW);
	void SetMaterial(ID3D11DeviceContext* dc, const Material& mat);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* shadowMap);
	void SetShadowTransform(ID3D11DeviceContext* dc, const XMFLOAT4X4& shadowTransform);

	void SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight pointLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirectionalLight dirLights[]);

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
// 		XMMATRIX world;
// 		XMMATRIX worldViewProj;
// 		XMMATRIX worldViewProjTex;
// 		XMMATRIX worldInvTranspose;
// 		XMMATRIX texTransform;
		Material mat;
		//XMFLOAT3 gEyePosW;
		//ID3D11ShaderResourceView* diffuseMap;
	};

	struct PS_CPERFRAMEBUFFER
	{
		PointLight pointLights[16];

		// 16 bytes
		UINT numPointLights;
		int padding2, padding3, padding4;

		DirectionalLight dirLights[4];

		// 16 bytes
		UINT numDirLights;
		int padding5, padding6, padding7;

		// Forms into a 4D vector
		XMFLOAT3 gEyePosW;
		float padding;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsBuffer;
		PS_CPEROBJBUFFER psBuffer;
		PS_CPERFRAMEBUFFER psPerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS
	ID3D11Buffer* vs_cBuffer;
	VS_CPEROBJBUFFER vs_cBufferVariables;

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
	//bool SetActive(ID3D11DeviceContext* dc) { return false; }
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
#pragma endregion SkyShaderEnd

#pragma region NormalMappedSkinnedShader
class NormalMappedSkinned : public IShader
{
public:
	NormalMappedSkinned();
	~NormalMappedSkinned();

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
		NormalMappedSkinned* ptr = static_cast<NormalMappedSkinned*>(p);
		_aligned_free(p);
	}

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	//bool SetActive(ID3D11DeviceContext* dc) { return false; }
	bool SetActive(ID3D11DeviceContext* dc);

	//void SetWorldViewProj(XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 projection);
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
		PointLight pointLights[16];

		// 16 bytes
		UINT numPointLights;
		int padding2, padding3, padding4;

		DirectionalLight dirLights[4];

		// 16 bytes
		UINT numDirLights;
		int padding5, padding6, padding7;

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

	// VS
	ID3D11Buffer* vs_cBuffer;
	VS_CPEROBJBUFFER vs_cBufferVariables;

	// VS skinned
	ID3D11Buffer* vs_cSkinnedBuffer;
	VS_CSKINNEDBUFFER vs_cSkinnedBufferVariables;

	// PS - per object
	ID3D11Buffer* ps_cPerObjBuffer;
	PS_CPEROBJBUFFER ps_cPerObjBufferVariables;

	// PS - per frame
	ID3D11Buffer* ps_cPerFrameBuffer;
	PS_CPERFRAMEBUFFER ps_cPerFrameBufferVariables;
};
#pragma endregion NormalMappedSkinnedShader

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
	ShadowShader* mShadowShader;

private:

	BYTE* LoadByteCode(char* fileName, UINT& size);

	//std::vector<ID3D11VertexShader*> mVShaders;
	//std::vector<ID3D11PixelShader*> mPShaders;

	std::map<std::string, ID3D11VertexShader*> mVertexShaders;
	std::map<std::string, ID3D11PixelShader*> mPixelShaders;

	std::vector<Shader*> mShaders;
};

#endif