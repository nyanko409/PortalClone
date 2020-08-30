#pragma once

#define FPS	60
#define TIME_PER_FRAME (1.0F / FPS)
#define SCREEN_WIDTH	(1080)			// ウインドウの幅
#define SCREEN_HEIGHT	(720)			// ウインドウの高さ

#define SAFE_RELEASE(p)			if(p){ p->Release(); p = nullptr; }
#define SAFE_DELETE(p)			if(p){ delete p;	 p = nullptr; }
#define SAFE_DELETE_ARRAY(p)	if(p){ delete[] p;	 p = nullptr; }

HWND GetWindow();
HINSTANCE GetInstance();
