#pragma once

#include "computeshader.h"


class SkinningCompute : public ComputeShader
{
public:
	void Init() override;
	void Uninit() override;
	void Run() override;

private:
	ID3D11Buffer*               m_pBuf0 = nullptr;
	ID3D11Buffer*               m_pBuf1 = nullptr;
	ID3D11Buffer*               m_pBufResult = nullptr;

	ID3D11ShaderResourceView*   m_pBuf0SRV = nullptr;
	ID3D11ShaderResourceView*   m_pBuf1SRV = nullptr;
	ID3D11UnorderedAccessView*  m_pBufResultUAV = nullptr;
};