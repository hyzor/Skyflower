#ifndef RENDERSTATES_H_
#define RENDERSTATES_H_

#include "d3dUtilities.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11RasterizerState* mDefaultRS;
	static ID3D11RasterizerState* mWireframeRS;
	static ID3D11RasterizerState* mNoCullRS;

	static ID3D11SamplerState* mLinearSS;
	static ID3D11SamplerState* mAnisotropicSS;
	static ID3D11SamplerState* mComparisonSS;

	static ID3D11DepthStencilState* mLessEqualDSS;
	static ID3D11DepthStencilState* mDefaultDSS;
	static ID3D11DepthStencilState* mDisabledDSS;

	static ID3D11BlendState* mDefaultBS;
};

#endif