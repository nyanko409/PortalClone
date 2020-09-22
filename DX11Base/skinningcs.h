#pragma once

#include "computeshader.h"


class SkinningCompute : public ComputeShader
{
public:
	struct mat4x4
	{
		float a1, a2, a3, a4;
		float b1, b2, b3, b4;
		float c1, c2, c3, c4;
		float d1, d2, d3, d4;
	};

	void Init() override;
	void Uninit() override;
	void Run() override;

	void UpdateMatrixBuffer(mat4x4 matrix);

private:
	ID3D11Buffer*               m_pBuf0 = nullptr;
	ID3D11Buffer*               m_pBuf1 = nullptr;
	ID3D11Buffer*               m_pBufResult = nullptr;

	ID3D11ShaderResourceView*   m_pBuf0SRV = nullptr;
	ID3D11ShaderResourceView*   m_pBuf1SRV = nullptr;
	ID3D11UnorderedAccessView*  m_pBufResultUAV = nullptr;
};
