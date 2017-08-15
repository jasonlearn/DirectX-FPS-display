#pragma once
#ifndef __BACKGROUND
#define __BACKGROUND
#define BITMAP_PICTURE "fox.bmp"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
void SetError(TCHAR*, ...);
HRESULT RestoreGraphics();
#endif