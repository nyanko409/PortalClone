#include "pch.h"
#include "skinningcs.h"
#include "renderer.h"


const UINT NUM_ELEMENTS = 1;


void SkinningCompute::Init()
{
	// create shader from file
	CreateComputeShader("skinning_compute.cso");

	// create structured buffers
	if (FAILED(CreateStructuredBuffer(sizeof(mat4x4), NUM_ELEMENTS, nullptr, &m_pBuf0, false)))			return;
	if (FAILED(CreateStructuredBuffer(sizeof(mat4x4), NUM_ELEMENTS, nullptr, &m_pBuf1, false)))			return;
	if (FAILED(CreateStructuredBuffer(sizeof(mat4x4), NUM_ELEMENTS, nullptr, &m_pBufResult, true)))		return;

	// create buffer views
	if (FAILED(CreateBufferSRV(m_pBuf0, &m_pBuf0SRV))) return;
	if (FAILED(CreateBufferSRV(m_pBuf1, &m_pBuf1SRV))) return;
	if (FAILED(CreateBufferUAV(m_pBufResult, &m_pBufResultUAV))) return;
}

void SkinningCompute::Uninit()
{
	ComputeShader::Uninit();

	SAFE_RELEASE(m_pBuf0);
	SAFE_RELEASE(m_pBuf1);
	SAFE_RELEASE(m_pBufResult);
	SAFE_RELEASE(m_pBuf0SRV);
	SAFE_RELEASE(m_pBuf1SRV);
	SAFE_RELEASE(m_pBufResultUAV);
}

void SkinningCompute::Run()
{
	// run
	ID3D11ShaderResourceView* aRViews[2] = { m_pBuf0SRV, m_pBuf1SRV };
	RunComputeShader(2, aRViews, nullptr, nullptr, 0, m_pBufResultUAV, 1, 1, 1);

	// check result
	// Read back the result from GPU, verify its correctness against result computed by CPU
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(m_pBufResult);
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		mat4x4 *p;
		CRenderer::GetDeviceContext()->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

		// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS
		// This is also a common trick to debug CS programs.
		p = (mat4x4*)MappedResource.pData;

		CRenderer::GetDeviceContext()->Unmap(debugbuf, 0);

		SAFE_RELEASE(debugbuf);
	}
}

void SkinningCompute::UpdateMatrixBuffer(mat4x4 matrix)
{
	WriteToBuffer(m_pBuf0, &matrix);
}
