#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <assert.h>

#include <vector>
#include <list>

#pragma warning(push)
#pragma warning(disable:4005)

#include <d3d11.h>
#include <d3dx11.h>
#include <DirectXMath.h>

#pragma warning(pop)

#include "dxmath_operator.h"


#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")


namespace dx = DirectX;
