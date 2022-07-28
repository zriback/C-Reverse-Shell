#include "images.h"
#include <windows.h>
#include <stdio.h>

#define QUALX 1280
#define QUALY 720

// NOTE: a lot of this code comes from https://docs.microsoft.com/en-us/windows/win32/gdi/capturing-an-image
// NOTE: Needs to be compiled with gcc with -lgdiplus and -lgdi32

// HWND hWnd in the window we are capturing an image of 
int captureImage(HWND hWnd, char * filename){
    HDC hdcWindow;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    DWORD dwBytesWritten = 0;
    DWORD dwSizeofDIB = 0;
    HANDLE hFile = NULL;
    char* lpbitmap = NULL;
    HANDLE hDIB = NULL;
    DWORD dwBmpSize = 0;

    // retrieve handle to display device content 
    hdcWindow = GetDC(hWnd);

    // Create a compatible DC (domain controller)
    hdcMemDC = CreateCompatibleDC(hdcWindow);
    if (!hdcMemDC){
        printf("Error creating hdcMemDC\n");
        return 1;
    }

    // Make process DPI aware to avoid wrongly sized screenshots when screen resolution changes
    SetProcessDPIAware();

    // create a compatible bitmap from the Window DC
    hbmScreen = CreateCompatibleBitmap(hdcWindow, QUALX, QUALY);
    if (!hbmScreen){
        printf("Error creating hmbScreen\n");
        return 1;
    }

    // Select the compatible bitmap into the memory DC
    SelectObject(hdcMemDC, hbmScreen);

    // use this stretch mode
    SetStretchBltMode(hdcMemDC, HALFTONE);

    // transfer bits into the memory DC
    StretchBlt(hdcMemDC, 0, 0, QUALX, QUALY, hdcWindow, 0 , 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SRCCOPY);

    // get the bitmap from the hbitmap
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    // bitmap file header and info header
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    // set all values for info header
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount+31) / 32) * 4 * bmpScreen.bmHeight;

    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char*)GlobalLock(hDIB);

    // get the bits from the bitmap and copy them into the buffer
    GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // create a file to save the capture into
    hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // get the total size of the file
    dwSizeofDIB = dwBmpSize + (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // offset to where our bitmap actually starts
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // size of the file
    bmfHeader.bfSize = dwSizeofDIB;

    // always set bfType to BM for bitmaps
    bmfHeader.bfType = 0x4D42; // that's BM

    // write data to the file
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    // unlock and free stuff
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    // close file handle
    CloseHandle(hFile);

    // Clean everything up

    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(hWnd, hdcWindow);

    return 0; // for normal exit
}

