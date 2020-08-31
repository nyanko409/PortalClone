#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <assert.h>

#include <memory>
#include <vector>
#include <list>

#pragma warning(push)
#pragma warning(disable:4005)

#include <d3d11.h>
#include <d3dx11.h>
#include <DirectXMath.h>

#pragma warning(pop)

#include <dinput.h>
#include <XAudio2.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "dxmath_operator.h"
#include "customMath.h"
#include "input.h"
#include "audio.h"

// directX
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

// direct input
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// audio
#pragma comment (lib, "xaudio2.lib") 
#pragma comment (lib, "winmm.lib") 


namespace dx = DirectX;
