#pragma once
#ifndef _DRAWLINE
#define _DRAWLINE
class DrawLine {
private:
	POINT pStart = { 0, 0 };
	POINT pEnd = { 0, 0 };
public:
	DrawLine();
	~DrawLine();
	void draw(int, DWORD*);
	void setPStart(POINT);
	void setPEnd(POINT);
	POINT getPStart();
	POINT getPEnd();
};
#endif