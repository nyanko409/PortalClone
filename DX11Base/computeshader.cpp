#include "pch.h"
#include "computeshader.h"
#include <d3dcompiler.h>
#include <io.h>
#include "main.h"
#include "renderer.h"


HRESULT ComputeShader::CreateComputeShader(const char* pSrcFile)
{
	FILE* file;
	long int fsize;

	file = fopen(pSrcFile, "rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	HRESULT hr = CRenderer::GetDevice()->CreateComputeShader(buffer, fsize, NULL, &m_shader);
	
	if (FAILED(hr))
	{
		MessageBox(GetWindow(), "Failed to create compute shader!", "Error!", MB_OK);
		return hr;
	}

	return hr;
}

void ComputeShader::RunComputeShader(UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews,
	ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
	ID3D11UnorderedAccessView* pUnorderedAccessView,
	UINT X, UINT Y, UINT Z)
{
	auto deviceContext = CRenderer::GetDeviceContext();

	deviceContext->CSSetShader(m_shader, nullptr, 0);
	deviceContext->CSSetShaderResources(0, nNumViews, pShaderResourceViews);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &pUnorderedAccessView, nullptr);
	if (pCBCS && pCSData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		deviceContext->Map(pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, pCSData, dwNumDataBytes);
		deviceContext->Unmap(pCBCS, 0);
		ID3D11Buffer* ppCB[1] = { pCBCS };
		deviceContext->CSSetConstantBuffers(0, 1, ppCB);
	}

	deviceContext->Dispatch(X, Y, Z);

	deviceContext->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView* ppUAViewnullptr[1] = { nullptr };
	deviceContext->CSSetUnorderedAccessViews(0, 1, ppUAViewnullptr, nullptr);

	ID3D11ShaderResourceView* ppSRVnullptr[2] = { nullptr, nullptr };
	deviceContext->CSSetShaderResources(0, 2, ppSRVnullptr);

	ID3D11Buffer* ppCBnullptr[1] = { nullptr };
	deviceContext->CSSetConstantBuffers(0, 1, ppCBnullptr);
}

HRESULT ComputeShader::CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut, bool resultBuffer)
{
	*ppBufOut = nullptr;

	D3D11_BUFFER_DESC desc = {};

	desc.ByteWidth = uElementSize * uCount;
	desc.StructureByteStride = uElementSize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	if (!resultBuffer)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return CRenderer::GetDevice()->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return CRenderer::GetDevice()->CreateBuffer(&desc, 0, ppBufOut);
}

HRESULT ComputeShader::CreateBufferSRV(ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		// This is a Structured Buffer
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}
	else
	{
		return E_INVALIDARG;
	}

	return CRenderer::GetDevice()->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

HRESULT ComputeShader::CreateBufferUAV(ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer
		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		// This is a Structured Buffer
		desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
		desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}
	else
	{
		return E_INVALIDARG;
	}

	return CRenderer::GetDevice()->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

ID3D11Buffer* ComputeShader::CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = nullptr;

	D3D11_BUFFER_DESC desc = {};
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(CRenderer::GetDevice()->CreateBuffer(&desc, nullptr, &debugbuf)))
	{
#if defined(_DEBUG)
		debugbuf->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Debug") - 1, "Debug");
#endif

		CRenderer::GetDeviceContext()->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
}

void ComputeShader::Uninit()
{
	SAFE_RELEASE(m_shader);
}
