#include "d3dUtilities.h"

// Must be included last!
#include "shared/debug.h"

//==============================================================
// D3D functions
//==============================================================
ID3D11ShaderResourceView* d3dHelper::CreateTexture2DArraySRV(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	std::vector<std::string>& filenames)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//

	HRESULT hr;

	UINT size = (UINT)filenames.size();

	D3D11_TEXTURE2D_DESC texElementDesc;

	std::vector<ID3D11Texture2D*> srcTex(size);
	for(UINT i = 0; i < size; ++i)
	{
		std::wstring texture0Path(filenames[0].begin(), filenames[0].end());
		std::wstring path(filenames[i].begin(), filenames[i].end());

		hr = CreateDDSTextureFromFileEx(device, path.c_str(), 0u, D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
			0, false, (ID3D11Resource**)&srcTex[i], nullptr, nullptr);

		if (srcTex[i] == NULL)
		{
			std::wostringstream ErrorStream;
			ErrorStream << "Failed to load texture " << path << std::endl << "(From CreateTexture2DArraySRV)";
			MessageBox(0, ErrorStream.str().c_str(), 0, 0);
		}

		// Check if this texture has the same format/dimensions as the first loaded texture
		else
		{
			if (i == 0)
			{
				srcTex[0]->GetDesc(&texElementDesc);
			}

			if (i > 0)
			{
				D3D11_TEXTURE2D_DESC curDesc;
				srcTex[i]->GetDesc(&curDesc);

				std::wostringstream ErrorMsg;
				bool error = false;

				// Format isn't the same
				if (curDesc.Format != texElementDesc.Format)
				{
					ErrorMsg << "Texture " << i << " format doesn't match texture " << 0 << " format!" << std::endl;
					ErrorMsg << "Texture " << 0 << ": " << texElementDesc.Format << std::endl;
					ErrorMsg << "Texture " << i << ": " << curDesc.Format << std::endl;
					error = true;
				}

				// Dimensions aren't the same
				else if (curDesc.Width != texElementDesc.Width || curDesc.Height != texElementDesc.Height)
				{
					ErrorMsg << "Texture " << i << " dimensions doesn't match texture " << 0 << " dimensions!" << std::endl;
					ErrorMsg << "Texture " << 0 << ": " << texElementDesc.Width << "x" << texElementDesc.Height << std::endl;
					ErrorMsg << "Texture " << i << ": " << curDesc.Width << "x" << curDesc.Height << std::endl;
					error = true;
				}

				// Mip levels doesn't match
				else if (curDesc.MipLevels != texElementDesc.MipLevels)
				{
					ErrorMsg << "Texture " << i << " mip levels doesn't match texture " << 0 << " mip levels!" << std::endl;
					ErrorMsg << "Texture " << 0 << ": " << texElementDesc.MipLevels << std::endl;
					ErrorMsg << "Texture " << i << ": " << curDesc.MipLevels << std::endl;
					error = true;
				}

				if (error)
				{
					ErrorMsg << std::endl << "Texture " << 0 << ": " << texture0Path << std::endl;
					ErrorMsg << "Texture " << i << ": " << path << std::endl;
					ErrorMsg << "(From CreateTexture2DArraySRV)" << std::endl;
					MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
				}
			}
		}
	}

	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = size;
	texArrayDesc.Format             = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	ID3D11Texture2D* texArray = 0;
	hr = device->CreateTexture2D( &texArrayDesc, 0, &texArray);

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	for(UINT texElement = 0; texElement < size; ++texElement)
	{
		// for each mipmap level...
		for(UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			hr = context->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);

			context->UpdateSubresource(texArray, 
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(srcTex[texElement], mipLevel);
		}
	}	

	//
	// Create a resource view to the texture array.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV = 0;
	hr = device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV);

	//
	// Cleanup--we only need the resource view.
	//

	ReleaseCOM(texArray);

	for(UINT i = 0; i < size; ++i)
		ReleaseCOM(srcTex[i]);

	return texArraySRV;
}

ID3D11ShaderResourceView* d3dHelper::CreateRandomTexture1DSRV(ID3D11Device* device)
{
	// 
	// Create the random data.
	//
	XMFLOAT4 randomValues[1024];

	for(int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024*sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	//HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));
	device->CreateTexture1D(&texDesc, &initData, &randomTex);

	//
	// Create the resource view.
	//
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* randomTexSRV = 0;
	//HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));
	device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV);

	ReleaseCOM(randomTex);

	return randomTexSRV;
}