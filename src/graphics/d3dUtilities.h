#ifndef D3DUTILITIES_H_
#define D3DUTILITIES_H_

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <d3d11.h>
#include <dxgi.h>

#include <DirectXMath.h>
#include <wrl/client.h>
#include <ppl.h>
#include <ppltasks.h>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "LightDefImpl.h"
#include "MathHelper.h"
#include <windows.h>

#include <DirectXTK/DDSTextureLoader.h>

//===============================================================
// Simple d3d error checker
//===============================================================
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
#define HR(x) (x)
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 

// Get the number of elements from an array
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

//===============================================================
// Macros
//===============================================================
// Macro for releasing COM objects
#define ReleaseCOM(x) { if (x) { x->Release(); x = 0; } }

// Macro for safe deletion
#define SafeDelete(x) { delete x; x = 0; }

//==============================================================
// D3D functions
//==============================================================
class d3dHelper
{
public:
	// Creates a 2D array with textures
	// Does not work with compressed formats
	static ID3D11ShaderResourceView* CreateTexture2DArraySRV(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		std::vector<std::string>& filenames);

	static ID3D11ShaderResourceView* CreateRandomTexture1DSRV(ID3D11Device* device);
};

#endif