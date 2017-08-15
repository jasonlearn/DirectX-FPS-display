#include "Includes.h"

/*
 * class Game handles directX initialization
 * BITMAP_PICUTRE is defined in Background.h for ease of alteration.
 *
 * @author Jason Chan A00698160
 */

// Default constructor for the game
// Initiates DrawLine for user interaction
Game::Game() {
	d = DrawLine();
}

// Constructor for the game with a handle to main window
// Initiates DrawLine for user interaction
// @param hWnd handle to window
Game::Game(HWND hWnd) {
	this->hWndMain = hWnd;
	d = DrawLine();
}

// Destructor for game
Game::~Game() {

}

// Set function for window handle
// @param hWnd handle to window
void Game::setHandle(HWND hWnd) {
	hWndMain = hWnd;
}

// Get function for window handle
HWND Game::getHandle() {
	return hWndMain;
}

/*
 * Function StaticWndProc, called when initializing.  Calls original WndProc
 * @param hWnd a handle to the window
 * @param uMessage the message 
 * @param wParam additional message information.  Content depends on the value of uMsg
 * @param lParam additional message information.  Content depends on the value of uMsg
 */
LRESULT CALLBACK Game::StaticWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
	Game* ptr;
	ptr = (Game*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (ptr != nullptr) {
		return ptr->WndProc(hWnd, uMessage, wParam, lParam);
	}
	else {
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
}

/* WndProc function, function for all input directed at the window
 * @param hWnd a handle to the window
 * @param uMessage the message to the window
 * @param wParam additional message information.  Content depends on the value of uMsg
 * @param lParam additional message information.  Content depends on the value of uMsg
 */
LRESULT CALLBACK Game::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
	D3DLOCKED_RECT rect;
	DWORD* pData;
	POINT pS, pE;
	// message case switch to update window
	switch (uMessage) {
	// creates window
	case WM_CREATE: {
		return 0;
	}
	// paint window, validates the client area
	case WM_PAINT: {
		ValidateRect(hWnd, NULL);
		return 0;
	}
	// reads user input, escape key is entered
	case WM_CHAR: {
		if (wParam == VK_ESCAPE)
		{
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		}
		return 0;
	}
	// mouse event message, calls drawLine functions
	case WM_LBUTTONDOWN:
		GetCursorPos(&pS);
		d.setPStart(pS);
		d.setPEnd(pS);
		return 0;
	// mouse event message, identify coordinates of cursor
	case WM_MOUSEMOVE:
		// mouse event message, for left button is held
		if (wParam == MK_LBUTTON) {
			GetCursorPos(&pE);
			d.setPEnd(pE);
		}
		return 0;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	} //end switch
}

/*
 * GameInit function, sets up directX and Window
 * Loads in the number.bmp for the frame counter
 * @return ok message is load was successful
 */
int Game::GameInit() {
	HRESULT r = 0;
	D3DSURFACE_DESC desc;
	LPDIRECT3DSURFACE9 pSurface = 0;

	// Direct3DCreate9 create directx object
	this->pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (this->pD3D == NULL) {
		SetError(_T("Could not create IDirect3D9 object"));
		return E_FAIL;
	}
	// InitDirect3DDevice(hWndMain, screen width, screen height, window mode (T = windowed, F = fullscreen), IDirect3DDevice9 object, pDevice)
	r = InitDirect3DDevice(this->hWndMain, SCREEN_WIDTH, SCREEN_HEIGHT, FALSE, D3DFMT_X8R8G8B8, this->pD3D, &this->pDevice);
	// handler for failed game initialization
	if (FAILED(r)) {
		SetError(_T("Initialization of the device failed"));
		return E_FAIL;
	}
	// GetBackBuffer(UINT, UINT, Buffer type, double pointer to backbuffer)
	r = this->pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	// handle for failed backbuffer retrieval failure
	if (FAILED(r)) {
		SetError(_T("Couldn't get backbuffer"));
	}
	pSurface->GetDesc(&desc);

	r = this->pDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &this->BitmapSurface, NULL);

	// load bitmap to surface
	r = LoadBitmapToSurface(_T(BITMAP_PICTURE), &pSurface, this->pDevice);
	// handler for failed bitmap load to surface
	if (FAILED(r))
		SetError(_T("Error loading bitmap"));
	// load surface from another surface with color conversion
	r = D3DXLoadSurfaceFromSurface(this->BitmapSurface, NULL, NULL, pSurface, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);
	// handler for failed load surface from one to another
	if (FAILED(r))
		SetError(_T("did not copy surface"));

	/* =============== FrameRate initializer =============== */
	frameController = FrameRate(pDevice);

	frameController.LoadNumber(_T("Number.bmp"), 8, 16);
	frameController.InitTiming();

	// return ok message if load was successful
	return S_OK;
}

/* 
 * GameLoop function, retrieves updated frameRate and renders it.
 * @return ok message is load was successful
 */
int Game::GameLoop() {
	frameController.FrameCount();
	this->Render();

	// Quits if user enters escape key
	if (GetAsyncKeyState(VK_ESCAPE))
		PostQuitMessage(0);

	// return ok message if load was successful
	return S_OK;
}

/* 
 * Exits the game, unload and release all resources
 * @return ok message is load was successful
 */
int Game::GameShutdown() {

	frameController.UnloadNumber();

	if (this->pVB)
		this->pVB->Release();

	if (this->pBackSurface)
		if (this->pDevice)
			this->pDevice->Release();

	if (this->pD3D)
		this->pD3D->Release();

	// return ok message if load was successful
	return S_OK;
}

/* 
 * LoadBitmapToSurface function to load bitmap to surface from memory
 * @param PathName of the bitmapa file to be loaded to the surface
 * @param ppSurface double pointer to the surface where bitmap is to be loaded
 * @param pDevice pointer to dx device
 *
 * @return ok message if load bitmap to surface was successful
 * @return fail message if loading of bitmap, initializatino of surface, or loading of file to surface was unsuccessful
 */
int Game::LoadBitmapToSurface(TCHAR* PathName, LPDIRECT3DSURFACE9* ppSurface, LPDIRECT3DDEVICE9 pDevice) {
	HRESULT r;
	HBITMAP hBitmap;
	BITMAP Bitmap;

	hBitmap = (HBITMAP)LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBitmap == NULL) {
		SetError(_T("Unable to load bitmap"));
		// return fail message to indicate load was unsuccessful
		return E_FAIL;
	}

	// retrieve header info used to create surface
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	// release unnecessary resources
	DeleteObject(hBitmap);
	// create surface for bitmap
	r = pDevice->CreateOffscreenPlainSurface(Bitmap.bmWidth, Bitmap.bmHeight, D3DFMT_X8R8G8B8, D3DPOOL_SCRATCH, ppSurface, NULL);

	if (FAILED(r)) {
		SetError(_T("Unable to create surface for bitmap load"));
		// return fail message to indicate load was unsuccessful
		return E_FAIL;
	}
	// load bitmap onto surface
	r = D3DXLoadSurfaceFromFile(*ppSurface, NULL, NULL, PathName, NULL, D3DX_DEFAULT, 0, NULL);
	if (FAILED(r)) {
		SetError(_T("Unable to laod file to surface"));
		// return fail message to indicate load was unsuccessful
		return E_FAIL;
	}
	// return ok message if load was successful
	return S_OK;
}

/*
 * Function InitDirect3DDevice to initialize directx device and set up buffers
 * @param hWndTarget targent window handler
 * @param Width of the device
 * @param Height of device
 * @param bWindowed boolean of windowmode
 * @param FullScreenFormat format of device of the screen
 * @param pD3D pointer of LPDIRECT3D9
 * @param ppDevice double pointer of device
 *
 * @return ok message is load was successful
 */
int Game::InitDirect3DDevice(HWND hWndTarget, int Width, int Height, BOOL bWindowed, D3DFORMAT FullScreenFormat, LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9* ppDevice) {
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;
	HRESULT r = 0;

	if (*ppDevice)
		(*ppDevice)->Release();

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	r = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(r)) {
		SetError(_T("Could not get display adapter information"));
		// return fail message to indicate load was unsuccessful
		return E_FAIL;
	}

	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = bWindowed ? d3ddm.Format : FullScreenFormat;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWndTarget;
	d3dpp.Windowed = bWindowed;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = bWindowed ? 0 : D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	r = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndTarget, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, ppDevice);
	if (FAILED(r)) {
		SetError(_T("Could not create the render device"));
		// return fail message to indicate load was unsuccessful
		return E_FAIL;
	}

	this->DeviceHeight = Height;
	this->DeviceWidth = Width;

	this->SavedPresParams = d3dpp;
	// return ok message if load was successful
	return S_OK;
}

/*
 * Render function to update surface.  Function is called during gameLoop and retrieves current stored backSurfaceBuffer
 *
 * @return ok message when render was successful
 * @return fail message if function was called but no device to be rendered, unable to validate, backbuff irretrievable, error ins surface setting
 */
int Game::Render() {
	HRESULT r;
	this->pBackSurface = 0;

	if (!this->pDevice) {
		SetError(_T("Cannot render because there is no device"));
		return E_FAIL;
	}
	this->pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 0, 0, 55), 1.0f, 0);
	r = ValidateDevice();
	if (FAILED(r)) {
		return E_FAIL;
	}
	r = this->pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &this->pBackSurface);
	if (FAILED(r)) {
		SetError(_T("Couldn't get backbuffer"));
	}
	// NULL for fullscreen
	r = this->pDevice->UpdateSurface(this->BitmapSurface, NULL, this->pBackSurface, NULL);
	if (FAILED(r)) {
		SetError(_T("Error setting surface to back surface"));
	}
	
	D3DLOCKED_RECT Locked;
	this->pBackSurface->LockRect(&Locked, 0, 0);
	frameController.PrintFrameRate(40, 50, TRUE, D3DCOLOR_ARGB(255, 255, 0, 255), (DWORD*)Locked.pBits, Locked.Pitch);
	d.draw(Locked.Pitch, (DWORD*)Locked.pBits);
	this->pBackSurface->UnlockRect();

	// Init Render in 3D
	this->pDevice->BeginScene();
	// End Render in 3D
	this->pDevice->EndScene();

	this->pDevice->Present(NULL, NULL, NULL, NULL);
	// return ok message if render was successful
	return S_OK;
}

/* 
 * Validate function to validate the device has successfully executed and is useable
 *
 * @return ok message if validateDevice was executed successfully
 */
HRESULT Game::ValidateDevice() {
	HRESULT r = 0;
	// To test the level of cooperative-ness of the device
	r = this->pDevice->TestCooperativeLevel();
	if (FAILED(r)) {
		// handle for device failure
		if (r == D3DERR_DEVICELOST)
			return E_FAIL;
		// handle if device has not reset
		if (r == D3DERR_DEVICENOTRESET) {
			//release back surface
			this->pBackSurface->Release();
			//reset device
			r = this->pDevice->Reset(&this->SavedPresParams);
			// handle if divce is not able to reset
			if (FAILED(r)) {
				SetError(_T("Could not reset device"));
				PostQuitMessage(E_FAIL);
				return E_FAIL;
			}
			r = this->pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &this->pBackSurface);
			if (FAILED(r)) {
				SetError(_T("Unable to reacquire back buffer"));
				PostQuitMessage(0);
				return E_FAIL;
			}
			this->pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
			this->RestoreGraphics();
		}
	}
	// return ok message when device successfully validated
	return S_OK;
}

/* 
 * Function to return a message is all initilization went well
 *
 * @return ok message when RestoreGraphics function executed successfully
 */
HRESULT Game::RestoreGraphics() {
	// return ok message if graphics resotred successfully
	return S_OK;
}