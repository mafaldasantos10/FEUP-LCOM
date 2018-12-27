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

// Represents a Bitmap
typedef struct {
    BitmapInfoHeader bitmapInfoHeader;
    unsigned char* bitmapData;
    bool colided;
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

int pix_map_move_pos(Bitmap * pad, Bitmap * background, Bitmap * arrow_right, Bitmap * arrow_left, Bitmap * arrow_up, Bitmap * arrow_down, Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great,Bitmap * cromossoma_idle, Bitmap * cromossoma_up, Bitmap * cromossoma_down, Bitmap * cromossoma_right, Bitmap * cromossoma_left,  Bitmap * pointer, Bitmap *power);

int arrowRate(int i);

void keyboardArrows();

int game(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);

void arrowProcessing(Bitmap * cromossoma_idle, Bitmap * pad, Bitmap * background, Bitmap * arrow_down, Bitmap * arrow_up, Bitmap * arrow_left, Bitmap * arrow_right, Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great, Bitmap * cromossoma_up, Bitmap * cromossoma_down, Bitmap * cromossoma_right, Bitmap * cromossoma_left, Bitmap * pointer, Bitmap *power);

void printDance(Bitmap * cromossoma_idle, Bitmap * cromossoma_up, Bitmap * cromossoma_down, Bitmap * cromossoma_right, Bitmap * cromossoma_left);

void powerUps(Bitmap * power, Bitmap * pad, Bitmap * arrow_right, Bitmap * arrow_left, Bitmap * arrow_up, Bitmap * arrow_down, Bitmap * cromossoma_idle, Bitmap * cromossoma_up, Bitmap * cromossoma_down, Bitmap * cromossoma_right, Bitmap * cromossoma_left, int xi, int yi, int yf);

int powerSpeed(int xi, int xf);

void changeDirect(Bitmap *power, Bitmap * pad, Bitmap * arrow_right, Bitmap * arrow_left, Bitmap * arrow_up, Bitmap * arrow_down, Bitmap * cromossoma_idle, Bitmap * cromossoma_up, Bitmap * cromossoma_down, Bitmap * cromossoma_right, Bitmap * cromossoma_left);
