#include "Includes.h"

/*
 * class Background handles background of project
 *
 * @Author	Jason Chan A00698160
 */

/*
 * Method to send error to the command line if input is invalid
 * @param szFormat the control to set the error description string for.
 * @param ... the value of the error description string, or null or Empty to remove the error
 */
void SetError(TCHAR* szFormat, ...) {
	TCHAR szBuffer[1024];
	va_list pArgList;

	va_start(pArgList, szFormat);

	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(char), szFormat, pArgList);

	va_end(pArgList);

	OutputDebugString(szBuffer);
	OutputDebugString(_T("\n"));
}

/*
 * Function to send message if bitmap was loaded successfully
 *
 * @return ok message if load was sucessful
 */
HRESULT RestoreGraphics() {
	return S_OK;
}