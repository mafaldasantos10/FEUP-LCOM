#pragma once

//STRUCTS
//////////////////////////////////////////////////////////////////

/** @defgroup Bitmap Bitmap
 * @{
 * Functions for manipulating bitmaps
 */
typedef enum {
    ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} Alignment;

typedef struct {
    unsigned short type; // specifies the file type
    unsigned int size; // specifies the size in bytes of the bitmap file
    unsigned int reserved; // reserved; must be 0
    unsigned int offset; // specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BitmapFileHeader;

typedef struct {
    unsigned int size; // specifies the number of bytes required by the struct
    int width; // specifies width in pixels
    int height; // specifies height in pixels
    unsigned short planes; // specifies the number of color planes, must be 1
    unsigned short bits; // specifies the number of bit per pixel
    unsigned int compression; // specifies the type of compression
    unsigned int imageSize; // size of image in bytes
    int xResolution; // number of pixels per meter in x axis
    int yResolution; // number of pixels per meter in y axis
    unsigned int nColors; // number of colors used by the bitmap
    unsigned int importantColors; // number of colors that are important
} BitmapInfoHeader;

/// Represents a Bitmap
typedef struct {
    BitmapInfoHeader bitmapInfoHeader;
    unsigned char* bitmapData;
} Bitmap;

//arrow
typedef struct {
    int currentX;
    int speed;
    bool active;
    int direction;
} Arrow;


//PROTOTYPES
//////////////////////////////////////////////////////////////////

/**
 * @brief Loads a bmp image
 *
 * @param filename Path of the image to load
 * @return Non NULL pointer to the image buffer
 */
Bitmap* loadBitmap(const char* filename);

/**
 * @brief Draws an unscaled, unrotated bitmap at the given position
 *
 * @param bitmap bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 * @param alignment image alignment
 */
void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment);

/**
 * @brief Destroys the given bitmap, freeing all resources used by it.
 *
 * @param bitmap bitmap to be destroyed
 */
void deleteBitmap(Bitmap* bmp);

void init_arrows();

int pix_map_move_pos(Bitmap * pad, Bitmap * background, Bitmap * arrowright, Bitmap * arrowleft, Bitmap * arrowup, Bitmap * arrowdown,  uint16_t xf, Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great,Bitmap * cromossoma1, Bitmap * cromossomaup, Bitmap * cromossomadown, Bitmap * cromossomaright, Bitmap * cromossomaleft);

int arrowRate(int i);

void keyboardArrows(Bitmap * pad, Bitmap * background);

void score(int i);

int game(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);

void arrowProcessing(Bitmap * cromossoma1, Bitmap * pad, Bitmap * background, Bitmap * arrowdown, Bitmap * arrowup, Bitmap * arrowleft, Bitmap * arrowright, Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great, Bitmap * cromossomaup, Bitmap * cromossomadown, Bitmap * cromossomaright, Bitmap * cromossomaleft);

void printScore(Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great);

void printDance(Bitmap * cromossoma1, Bitmap * cromossomaup, Bitmap * cromossomadown, Bitmap * cromossomaright, Bitmap * cromossomaleft);
