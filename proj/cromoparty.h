#pragma once

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type - 2 bytes
    DWORD bfSize;  //specifies the size in bytes of the bitmap file - 4 bytes
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
} BITMAPFILEHEADER;

typedef struct BITMAPV5HEADER { // DWORD, LONG: 4 bytes WORD: 2 bytes
	DWORD bV5Size; // this header size
	LONG bV5Width;
	LONG bV5Height; // If negative, first row is top
	WORD bV5Planes;
	WORD bV5BitCount; // Bits per pixel
	DWORD bV5Compression;
	DWORD bV5SizeImage;
	LONG bV5XPelsPerMeter;
	LONG bV5YPelsPerMeter;
	DWORD bV5ClrUsed;
	DWORD bV5ClrImportant;
	DWORD bV5RedMask;
	DWORD bV5GreenMask;
	DWORD bV5BlueMask;
	DWORD bV5AlphaMask;
	DWORD bV5CSType;
	CIEXYZTRIPLE bV5Endpoints; // 36 bytes
	DWORD bV5GammaRed;
	DWORD bV5GammaGreen;
	DWORD bV5GammaBlue;
	DWORD bV5Intent;
	DWORD bV5ProfileData;
	DWORD bV5ProfileSize;
	DWORD bV5Reserved;
} BITMAPV5HEADER;
