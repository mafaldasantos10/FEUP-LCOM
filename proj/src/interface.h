#pragma once

/** @defgroup Interface Interface
 * @{
 *
 * Functions to manipulate the Interface of the game (Video Card)
 */

//STRUCTS
//////////////////////////////////////////////////////////////////

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

/** @brief Bitmap Struct */
typedef struct {
    BitmapInfoHeader bitmapInfoHeader;
    unsigned char* bitmapData;
    bool colided;
} Bitmap;

/** @brief Struct that has all images to be loaded */
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
    Bitmap* cromossomaY_idle;
    Bitmap* cromossomaY_down;
    Bitmap* cromossomaY_left;
    Bitmap* cromossomaY_right;
    Bitmap* cromossomaY_up;
    Bitmap* cromossomaX_idle;
    Bitmap* cromossomaX_down;
    Bitmap* cromossomaX_left;
    Bitmap* cromossomaX_right;
    Bitmap* cromossomaX_up;
    Bitmap* perfect;
    Bitmap* great;
    Bitmap* okay;
    Bitmap* miss;
    Bitmap* power;
    Bitmap* pointer;
    Bitmap* singleplayer_selected;
    Bitmap* multiplayer_selected;
    Bitmap* instructions_selected;
    Bitmap* highscores_selected;
    Bitmap* exit_selected;
    Bitmap* singleplayer_not_selected;
    Bitmap* multiplayer_not_selected;
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
    Bitmap* waiting;
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
    Bitmap* slash;
    Bitmap* colon;
    Bitmap* cromossomaY;
    Bitmap* cromossomaX;
    Bitmap* scoreY;
    Bitmap* scoreX;
} Images;


//GLOBAL VARIABLES
//////////////////////////////////////////////////////////////////
extern Images images;
extern char path[50];


//PROTOTYPES
//////////////////////////////////////////////////////////////////

/**
 * @brief Returns information on the input VBE mode, including screen dimensions, color depth and VRAM physical address.
 * 
 * @param mode Mode whose information should be returned 
 * @param vmi_p Address of vbe_mode_info_t structure to be initialized 
 * @return 0 on success, non-zero otherwise
 */
int get_mode_info(uint16_t mode, vbe_mode_info_t * vmi_p);

/**
 * @brief Double buffer function. Copies contents of the second buffer to video-mem
 */
void double_buffer_to_video_mem();

/**
 * @brief Gets the horizontal resolution
 * @return Horizontal resolution
 */
int get_horizontal_resolution();

/**
 * @brief Gets the vertical resolution
 * @return Vertical resolution
 */
int get_vertical_resolution();

/**
 * @brief Loads all the images used in the project
 */
void loadImages();

/**
 * @brief Deletes all the images used
 */
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
 * @param bmp Bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 * @param alignment Image alignment
 */
void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment);

/**
 * @brief Destroys the given bitmap, freeing all resources used by it.
 *
 * @param bmp Bitmap to be destroyed
 */
void deleteBitmap(Bitmap* bmp);

char* appendString(char* s);

/**@}*/
