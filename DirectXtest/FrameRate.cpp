#include "Includes.h"

/*
 * Class FrameRate to handle FPS calculation and display
 *
 * @Author	Jason Chan A00698160
 */

// Default constructor for FrameRate
FrameRate::FrameRate() {
}

/*
 * Constructor for FrameRate that sets up the device
 *
 * @param dev, device that was created
 */
FrameRate::FrameRate(LPDIRECT3DDEVICE9 dev) {
	this->pDevice = dev;
}

// Default destructor for FrameRate
FrameRate::~FrameRate() {

}

// Function FrameCount, used to get current frame count
void FrameRate::FrameCount() {
	INT64 NewCount = 0;
	static INT64 LastCount = 0;
	INT64 Difference = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&NewCount);

	if (NewCount == 0)
		SetError(_T("The system does not support high resolution timing"));

	this->incFrame_Counter();

	Difference = NewCount - LastCount;

	if (Difference >= this->frequency) {
		this->setFrameRate(this->frame_counter);
		this->setFrameRate_Counter(0);
		LastCount = NewCount;
	}
}

/*
 * Function InitTiming, used to initiate the timer for frame count
 *
 * @return message ok if initiation was successful
*/
HRESULT FrameRate::InitTiming() {
	QueryPerformanceFrequency((LARGE_INTEGER*)&this->frequency);
	if (this->frequency == 0) {
		SetError(_T("The system does not support high resolution timing"));
		return E_FAIL;
	}
	return S_OK;
}

// Fuction incFrame_Counter, used to increment the frame count by 1 each time it's called
void FrameRate::incFrame_Counter() {
	this->frame_counter++;
}

/* ================== Accessor and Mutator Functions =====================*/

// Function setFrameRate_Counter, mutator to set frame_counter
// @param n, frame rate counter
void FrameRate::setFrameRate_Counter(int n) {
	this->frame_counter = n;
}
// Function getFrameRate_Counter, accessor to get frame_counter
int FrameRate::getFrameRate_Counter() {
	return frame_counter;
}
// Function setFrameRate, mutator to set frame_rate
// @param n, frame rate per second to be set
void FrameRate::setFrameRate(int n) {
	this->Frame_rate = n;
}
// Function setFrequency, mutator to set current frequency
// @param n, frequency to be set
void FrameRate::setFrequency(int n) {
	this->frequency = n;
}

/*
 * Function LoadNumber, used to load the number bitmap file to memory
 *
 * @param strPathName, path of bitmap file
 * @param LetterWidth, width of the number in bitmap
 * @param LetterHeight, height of the number in bitmap
 * 
 * @return message OK if function is successfully executed
 * else return message fail if invalid pathname, file length <= 0, or fail to load bitmap.
 */
HRESULT FrameRate::LoadNumber(TCHAR* strPathName, int LetterWidth, int LetterHeight) {
	// To ensure path of file is correct
	if (!strPathName)
		return E_FAIL;

	// To ensure file length is greater than 0
	if (!LetterWidth || !LetterHeight)
		return E_FAIL;

	HRESULT r = 0;

	// Load the bitmap into memory
	r = LoadBitmapToSurface(strPathName, &this->g_pNumberSurface, this->pDevice);
	// Hendler when number bitmap fails to load
	if (FAILED(r))
	{
		SetError(_T("Unable to load number bitmap"));
		return E_FAIL;
	}

	// To hold information of the number surface
	D3DSURFACE_DESC d3dsd;

	// To get information about the number surface
	g_pNumberSurface->GetDesc(&d3dsd);

	// To update dimensions accordingly to number.bmp
	g_NumberWidth = d3dsd.Width;
	g_NumberHeight = d3dsd.Height;
	g_NumberLetterWidth = LetterWidth;
	g_NumberLetterHeight = LetterHeight;

	// Function to calculate numbers per row in number.bmp
	g_NumberLettersPerRow = g_NumberWidth / g_NumberLetterWidth;

	// Boolean to set flag to load number
	g_bNumberLoaded = TRUE;

	return S_OK;
}

// Function UnloadNumber, to load number from memory to surface
HRESULT FrameRate::UnloadNumber() {
	// To check if number has already been loaded
	if (this->g_pNumberSurface)
	{
		// Release number the surface
		g_pNumberSurface->Release();
		// set numbersurface pointer to null
		g_pNumberSurface = 0;
		// Boolean to set flag to load number
		this->g_bNumberLoaded = FALSE;
	}

	// Method to return message if load was successful
	return S_OK;
}

/*
* Function PrintChar: used to print a character from loaded number to the surface
*
* @param x, x-coord in client area
* @param y, y coord in client area
* @param char, char of message to be printed
* @param bTransparent, boolean to state if display is transparent or not
* @param ColorKey color key
* @param pDestData pointer to destination of the data
* @param DestPitch, destination of pitch
*/
void FrameRate::PrintChar(int x, int y, char Character, BOOL bTransparent, D3DCOLOR ColorKey, DWORD* pDestData, int DestPitch) {
	HRESULT r = 0;

	div_t Result; // Variable to hold the result of division
	int OffsetX = 0, OffsetY = 0; // offset number image

	POINT LetterDestPoint = { 0, 0 };	// The destination point for the number
	RECT LetterRect = { 0, 0, 0, 0 };	// The source rectangle for the number

	// Exit if number has not been loaded
	if (!this->g_bNumberLoaded)
		return;

	// The characters are specified in ASCII code, which begins at 32 so
	// we want to decrement this value by 32 to make it zero based
	Character -= 32;

	// To ensure the number to be compute is not equal to 0
	// Else there will be critical error
	if (Character == 0)
		return;

	// Divide the character code by the number of letters per row.
	Result = div(Character, this->g_NumberLettersPerRow);
	// Get the horizontal offset by multiplying the remainder by the width of the Letter
	OffsetX = Result.rem * this->g_NumberLetterWidth;
	// Get the vertical offset by multiplying the quotient by the height of the letter
	OffsetY = Result.quot * this->g_NumberLetterHeight;

	// Fill in the source rectangle with the computed offsets
	SetRect(&LetterRect, OffsetX, OffsetY,
		OffsetX + this->g_NumberLetterWidth, OffsetY + this->g_NumberLetterHeight);

	// Fill in the destination point
	LetterDestPoint.x = x;
	LetterDestPoint.y = y;

	D3DLOCKED_RECT LockedNumber; // Holds info about the number surface

	// Lock the source surface
	r = this->g_pNumberSurface->LockRect(&LockedNumber, 0, D3DLOCK_READONLY);
	if (FAILED(r))
	{
		SetError(_T("Couldn't lock number surface for PrintChar()"));
		return;
	}

	// Get a DWORD pointer to each surface
	DWORD* pNumData = (DWORD*)LockedNumber.pBits;

	// Convert the BYTE pitch pointer to a DWORD ptr
	LockedNumber.Pitch /= 4;
	DestPitch /= 4;

	// Compute the offset into the number
	int NumOffset = OffsetY * LockedNumber.Pitch + OffsetX;
	// Compute the offset into the destination surface
	int DestOffset = y * DestPitch + x;

	// Loop for each row in the letter
	for (int cy = 0; cy < g_NumberLetterHeight; cy++)
	{
		// Loop for each column in the letter
		for (int cx = 0; cx < g_NumberLetterWidth; cx++)
		{
			if (bTransparent)
			{
				// If this number pixel is not transparent
				if (pNumData[NumOffset] != ColorKey)
				{
					// Then copy the pixel to the destination
					pDestData[DestOffset] = pNumData[NumOffset];
				}

				// Increment the offsets to the next pixel
				NumOffset++;
				DestOffset++;
			}
			else
				pDestData[DestOffset] = pNumData[NumOffset];
		}

		// Move the offsets to the start of the next row
		DestOffset += DestPitch - g_NumberLetterWidth;
		NumOffset += LockedNumber.Pitch - g_NumberLetterWidth;
	}

	// Unlock the surface
	g_pNumberSurface->UnlockRect();

}

/*
* Function PrintString used to print a single string
*
* @param x, x-coord in client area
* @param y, y coord in client area
* @param string, string of message to be printed
* @param bTransparent, boolean to state if display is transparent or not
* @param ColorKey color key
* @param pDestData pointer to destination of the data
* @param DestPitch, destination of pitch
*/
void FrameRate::PrintString(int x, int y, char* String, BOOL bTransparent, D3DCOLOR ColorKey, DWORD* pDestData, int DestPitch) {
	// Loop for each character in the string
	for (UINT i = 0; i < strlen(String); i++)
	{
		// Print the current character
		PrintChar(x + (this->g_NumberLetterWidth * i), y, String[i], bTransparent, ColorKey, pDestData, DestPitch);
	}
}

/*
 * Prints the frame rate to the screen
 *
 * @param x, x-coord in client area
 * @param y, y coord in client area
 * @param bTransparent, boolean to state if display is transparent or not
 * @param ColorKey color key
 * @param pDestData pointer to destination of the data
 * @param DestPitch, destination of pitch
 */
void FrameRate::PrintFrameRate(int x, int y, BOOL bTransparent, D3DCOLOR ColorKey, DWORD* pDestData, int DestPitch)
{
	char string[10]; // String to hold frameRate

	// Zero out the string
	ZeroMemory(&string, sizeof(string));

	// Convert the frame rate to a string
	_itoa_s((int)this->Frame_rate, string, 10);

	// Output the string to the back surface
	PrintString(x + 20, y, string, TRUE, D3DCOLOR_ARGB(0, 255, 0, 255), pDestData, DestPitch);
	PrintString(x - 10, y, "FPS:", TRUE, D3DCOLOR_ARGB(0, 255, 0, 255), pDestData, DestPitch);
}

/* 
 * Loads the bitmap to the front surface from the buffer
 * @param PathName of bitmap
 * @param ppSurface pointer to surface
 * @param pDevice pointer to device
 *
 * @return ok message if bitmap is successfully loaded to surface
 * @return fail message if unable to load bitmap, create surface, nor load file to surface
 */
int FrameRate::LoadBitmapToSurface(TCHAR* PathName, LPDIRECT3DSURFACE9* ppSurface, LPDIRECT3DDEVICE9 pDevice) {
	HRESULT r;
	HBITMAP hBitmap;
	BITMAP Bitmap;

	hBitmap = (HBITMAP)LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBitmap == NULL) {
		SetError(_T("Unable to load bitmap"));
		return E_FAIL;
	}
	// retrieve header info used to create surface
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	// release unnecessary resources
	DeleteObject(hBitmap);
	// create surface for bitmap
	r = pDevice->CreateOffscreenPlainSurface(Bitmap.bmWidth, Bitmap.bmHeight, D3DFMT_X8R8G8B8, D3DPOOL_SCRATCH, ppSurface, NULL);
	// To handle if bitmap was unable to load
	if (FAILED(r)) {
		SetError(_T("Unable to create surface for bitmap load"));
		return E_FAIL;
	}
	// Fuction to load bitmap onto surface
	r = D3DXLoadSurfaceFromFile(*ppSurface, NULL, NULL, PathName, NULL, D3DX_DEFAULT, 0, NULL);
	if (FAILED(r)) {
		SetError(_T("Unable to load file to surface"));
		return E_FAIL;
	}
	// Function to return message if load was successful
	return S_OK;
}