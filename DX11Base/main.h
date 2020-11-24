#pragma once

#define FPS	60
#define TIME_PER_FRAME (1.0F / FPS)
#define SCREEN_WIDTH	(1024)			// ウインドウの幅
#define SCREEN_HEIGHT	(1024)			// ウインドウの高さ

HWND GetWindow();
HINSTANCE GetInstance();
