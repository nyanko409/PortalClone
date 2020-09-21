#include "pch.h"
#include "skinningcs.h"
#include "renderer.h"


const UINT NUM_ELEMENTS = 1024;

struct BufType
{
	int i;
	float f;
};
BufType g_vBuf0[NUM_ELEMENTS];
BufType g_vBuf1[NUM_ELEMENTS];

void SkinningCompute::Init()
{
	// init data
	for (int i = 0; i < NUM_ELEMENTS; ++i)
	{
		g_vBuf0[i].i = i;
		g_vBuf0[i].f = (float)i;
		g_vBuf1[i].i = i;
		g_vBuf1[i].f = (float)i;
	}

	// create shader from file
	CreateComputeShader("skinning_compute.cso");

	// create structured buffers
	if (FAILED(CreateStructuredBuffer(sizeof(BufType), NUM_ELEMENTS, &g_vBuf0[0], &m_pBuf0)))			return;
	if (FAILED(CreateStructuredBuffer(sizeof(BufType), NUM_ELEMENTS, &g_vBuf1[0], &m_pBuf1)))			return;
	if (FAILED(CreateStructuredBuffer(sizeof(BufType), NUM_ELEMENTS, nullptr, &m_pBufResult)))			return;

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
	RunComputeShader(2, aRViews, nullptr, nullptr, 0, m_pBufResultUAV, NUM_ELEMENTS, 1, 1);

	// check result
	// Read back the result from GPU, verify its correctness against result computed by CPU
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(m_pBufResult);
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		BufType *p;
		CRenderer::GetDeviceContext()->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

		// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS
		// This is also a common trick to debug CS programs.
		p = (BufType*)MappedResource.pData;

		CRenderer::GetDeviceContext()->Unmap(debugbuf, 0);

		SAFE_RELEASE(debugbuf);
	}
}
