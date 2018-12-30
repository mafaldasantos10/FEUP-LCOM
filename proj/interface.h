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

// Struct that has all images to be loaded
typedef struct {
    Bitmap* one_s;
    Bitmap* two_s;
    Bitmap* three_s;
    Bitmap* four_s;
    Bitmap* five_s;
    Bitmap* six_s;
    Bitmap* seven_s;
    Bitmap* eight_s;
    Bitmap* nine_s;
    Bitmap* zero_s;
    Bitmap* one;
    Bitmap* two;
    Bitmap* three;
    Bitmap* four;
    Bitmap* five;
    Bitmap* six;
    Bitmap* seven;
    Bitmap* eight;
    Bitmap* nine;
    Bitmap* zero;
    Bitmap* background;
    Bitmap* pad;
    Bitmap* arrow_up;
    Bitmap* arrow_right;
    Bitmap* arrow_down;
    Bitmap* arrow_left;
    Bitmap* cromossoma_idle;
    Bitmap* cromossoma_down;
    Bitmap* cromossoma_left;
    Bitmap* cromossoma_right;
    Bitmap* cromossoma_up;
    Bitmap* perfect;
    Bitmap* great;
    Bitmap* okay;
    Bitmap* miss;
    Bitmap* power;
    Bitmap* pointer;
    Bitmap* start_selected;
    Bitmap* instructions_selected;
    Bitmap* highscores_selected;
    Bitmap* exit_selected;
    Bitmap* start_not_selected;
    Bitmap* instructions_not_selected;
    Bitmap* highscores_not_selected;
    Bitmap* exit_not_selected;
    Bitmap* menu;
    Bitmap* instructions;
    Bitmap* highscores;
    Bitmap* pause;
    Bitmap* end;
    Bitmap* extra;
    Bitmap* name_menu;
    Bitmap* a;
    Bitmap* b;
    Bitmap* c;
    Bitmap* d;
    Bitmap* e;
    Bitmap* f;
    Bitmap* g;
    Bitmap* h;
    Bitmap* i;
    Bitmap* j;
    Bitmap* k;
    Bitmap* l;
    Bitmap* m;
    Bitmap* n;
    Bitmap* o;
    Bitmap* p;
    Bitmap* q;
    Bitmap* r;
    Bitmap* s;
    Bitmap* t;
    Bitmap* u;
    Bitmap* v;
    Bitmap* w;
    Bitmap* x;
    Bitmap* y;
    Bitmap* z;
    Bitmap* exc;
    Bitmap* inter;
} Images;


//GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////
extern Images images;


//PROTOTYPES
//////////////////////////////////////////////////////////////////

int get_mode_info(uint16_t mode, vbe_mode_info_t * vmi_p);

void double_buffer_to_video_mem();

int get_horizontal_resolution();

int get_vertical_resolution();

void loadImages();

void deleteImages();

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
