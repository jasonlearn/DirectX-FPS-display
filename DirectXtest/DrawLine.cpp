#include "Includes.h"

/*
 * Class DrawLine to allow user to draw a line on surface
 *
 * @Author	Jason Chan A00698160
 */

// Default constructor for DrawLine
DrawLine::DrawLine() {
}

// Default destructor for DrawLine
DrawLine::~DrawLine() {
}

/* Function draw, sets pixels on the screen relative to start and end point of the line
 *
 * @param Pitch number of bytes per line
 * @param pData pointer to pixel data
 */
void DrawLine::draw(int Pitch, DWORD* pData) {
	int length = SCREEN_WIDTH * SCREEN_HEIGHT;
	for (double i = 0; i < length; i++) {
		int x = pStart.x + (i / length) * (pEnd.x - pStart.x);
		int y = pStart.y + (i / length) * (pEnd.y - pStart.y);
		DWORD Offset = y * Pitch / 4 + x;
		pData[Offset] = D3DCOLOR_XRGB(255, 0, 0);
	}
}

/* ================== Accessor and Mutator Functions ================= */

// Mutator to set start point
// @param p, the point to be set to start
void DrawLine::setPStart(POINT p) {
	pStart = p;
}

// Accessor to get start point
// @return the point of start
POINT DrawLine::getPStart() {
	return pStart;
}

// Mutator to set end point
// @param p, the point to be set to end
void DrawLine::setPEnd(POINT p) {
	pEnd = p;
}

// Accessor to get end point
// @return pEnd, the end point
POINT DrawLine::getPEnd() {
	return pEnd;
}