#pragma once
#ifndef __FRAMERATE
#define __FRAMERATE
class FrameRate {
private:
	int frame_counter = 0;
	int Frame_rate = 0;
	int frequency = 0;

	LPDIRECT3DDEVICE9 pDevice = 0;

	int g_NumberWidth = 0;
	int g_NumberHeight = 0;
	int g_NumberLetterWidth = 0;
	int g_NumberLetterHeight = 0;
	int g_NumberLettersPerRow = 0;
	LPDIRECT3DSURFACE9 g_pNumberSurface = 0;
	BOOL g_bNumberLoaded = FALSE;
public:
	FrameRate();
	FrameRate(LPDIRECT3DDEVICE9);
	~FrameRate();
	void FrameCount();
	void incFrame_Counter();
	HRESULT InitTiming();
	void setFrameRate_Counter(int);
	int getFrameRate_Counter();
	void setFrameRate(int);
	void setFrequency(int);
	HRESULT LoadNumber(TCHAR*, int, int);
	HRESULT UnloadNumber();
	void PrintChar(int, int, char, BOOL, D3DCOLOR, DWORD*, int);
	void PrintString(int, int, char*, BOOL, D3DCOLOR, DWORD*, int);
	void PrintFrameRate(int, int, BOOL, D3DCOLOR, DWORD*, int);
	int LoadBitmapToSurface(TCHAR*, LPDIRECT3DSURFACE9*, LPDIRECT3DDEVICE9);
};

#endif
