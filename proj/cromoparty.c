#include <machine/int86.h>
#include <lcom/lcf.h>

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"
#include "i8254.h"
#include "cromoparty.h"
#include "keyboard.h"
#include "interface.h"


//VARIABLE INITIALIZATION
bool keep = true;
int scoreCounter = 0;


//FUNCTIONS
//////////////////////////////////////////////////////////////////

Bitmap* loadBitmap(const char* filename) 
{
    // allocating necessary size
    Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

    // open filename in read binary mode
    FILE *filePtr;
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read the bitmap file header
    BitmapFileHeader bitmapFileHeader;
    fread(&bitmapFileHeader, 2, 1, filePtr);

    // verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.type != 0x4D42) 
    {
        fclose(filePtr);
        return NULL;
    }

    int rd;
    do {
        if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
            break;
    } while (0);

    if (rd != 1) 
    {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    unsigned char* bitmapImage = (unsigned char*) malloc(bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) 
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) 
    {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;

    return bmp;
}

//////////////////////////////////////////////////////////////////

void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment) 
{
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > res_x || y + height < 0 || y > res_y)
        return;

    int xCorrection = 0;
    if (x < 0) 
    {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > res_x)
            drawWidth = res_x;
    } 
    else if (x + drawWidth >= res_x) 
    {
        drawWidth = res_x - x;
    }

    unsigned char* bufferStartPos;
    unsigned char* imgStartPos;

    int i;
    for (i = 0; i < height; i++) 
    {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= res_y)
            continue;

        bufferStartPos = double_buffer;

        bufferStartPos += x * 4 + pos * res_x * 4;

        imgStartPos = (unsigned char*)(bmp->bitmapData) + xCorrection * 4 + i * width * 4;

        for(int j = 0; j < drawWidth * 4; j += 4)
        {
            if(imgStartPos[j] != 0xF8 && imgStartPos[j + 1] != 0x0F && imgStartPos[j + 2] != 0x1F && imgStartPos[j + 3] != 0xFF)
            {
                bufferStartPos[j] = imgStartPos[j];
                bufferStartPos[j + 1] = imgStartPos[j + 1];
                bufferStartPos[j + 2] = imgStartPos[j + 2];
                bufferStartPos[j + 3] = imgStartPos[j + 3];
            }
        }
    }
}

//////////////////////////////////////////////////////////////////

void deleteBitmap(Bitmap* bmp)
{
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}

//////////////////////////////////////////////////////////////////

int pix_map_move_pos(Bitmap * pad, Bitmap * background, Bitmap * arrow, Bitmap * cromossoma1, uint16_t yf, int16_t speed, uint8_t fr_rate)
{
    if (timer_counter % (sys_hz() / fr_rate) == 0)
    {
        drawBitmap(background, 0, 0, ALIGN_LEFT);
        drawBitmap(pad, 438, 393, ALIGN_LEFT);
        drawBitmap(cromossoma1, 412, 20, ALIGN_LEFT);

        if (y - speed < yf)
        {
            y = yf;
            keep = false;
        }
        else
        {
            y -= speed;
        }
        
        if (y <= yf)
        {
            keep = false;
        }

        drawBitmap(arrow, 438, y, ALIGN_LEFT);

        double_buffer_to_video_mem();
    }
   
    return 0;
}

//////////////////////////////////////////////////////////////////

int arrowRate() 
{
    int rate = rand() % 3;
    speed = 3 + rand() % 15;
    arrow = rand() % 3;

    switch (rate) 
    {
      case 0:
        fr_rate = 15;
        break;
      case 1:
        fr_rate = 20;
        break;
      case 2:
        fr_rate = 30;
        break;
      case 3:
        fr_rate = 60;
        break;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////

void keyboardArrows(Bitmap * cromossomaup, Bitmap * pad, Bitmap * background,  Bitmap * cromossoma1, Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great)
{
    drawBitmap(background, 0, 0, ALIGN_LEFT);
    drawBitmap(pad, 438, 393, ALIGN_LEFT);
    drawBitmap(cromossomaup, 412, 20, ALIGN_LEFT);
    double_buffer_to_video_mem();

    if (status == 0x48)
    {
        if (up)
        {
            keep = false;
            score(okay, miss, perfect, great);
            double_buffer_to_video_mem();
            sleep(1);          
            up = false;
        }
    }

    if (status == 0x4d)
    {
        if (right)
        {
            keep = false;
            score(okay, miss, perfect, great);
            double_buffer_to_video_mem();
            sleep(1);
            right = false;
        }
    }

    if (status == 0x50)
    {
        if (down)
        {
            keep = false;
            score(okay, miss, perfect, great);
            double_buffer_to_video_mem();
            sleep(1);
            down = false;
        }
    } 

    if (status == 0x4b)
    {
        if (left)
        {
            keep = false;
            score(okay, miss, perfect, great);
            double_buffer_to_video_mem();
            sleep(1);
            left = false;
        }
    }

    drawBitmap(background, 0, 0, ALIGN_LEFT);
    drawBitmap(pad, 438, 393, ALIGN_LEFT);
    drawBitmap(cromossoma1, 412, 20, ALIGN_LEFT);
    double_buffer_to_video_mem();
}

//////////////////////////////////////////////////////////////////

void score(Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great)
{
    if (abs(393 - y) < 10)
    {
        drawBitmap(perfect, 360, 298, ALIGN_LEFT);
        scoreCounter += 20;
        return;
    }
    else if (abs(393 - y) < 40)
    {
        drawBitmap(great, 360, 298, ALIGN_LEFT);
        scoreCounter += 10;
        return;
    }
    else if (abs(393 - y) < 100)
    {
        drawBitmap(okay, 360, 298, ALIGN_LEFT);
        scoreCounter += 5;
        return;
    }
    else
    {
        drawBitmap(miss, 360, 298, ALIGN_LEFT);
        return;
    }
}
