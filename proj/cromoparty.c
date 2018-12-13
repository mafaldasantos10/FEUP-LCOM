#include <lcom/lcf.h>
#include <lcom/timer.h>

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
uint16_t y = 768;
uint8_t fr_rate = 30;
uint16_t speed = 5;
int arrow = 0;
bool up = false;
bool down = false;
bool left = false;
bool right = false;


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

        uint32_t *ptr =(uint32_t*) imgStartPos;
        uint32_t *buff =(uint32_t*) bufferStartPos;

        for(int j = 0; j < drawWidth; j ++)
        {
            if(ptr[j] != 0x1f0ff8)
            {
                buff[j] = ptr[j];
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


int game(uint8_t bit_no_kb)
{
    Bitmap * background = loadBitmap("/home/lcom/labs/proj/bitmap/discof.bmp"); 
    drawBitmap(background, 0, 0, ALIGN_LEFT);
    Bitmap * pad = loadBitmap("/home/lcom/labs/proj/bitmap/pad.bmp"); 
    drawBitmap(pad, 430, 393, ALIGN_LEFT);
    Bitmap * arrowup = loadBitmap("/home/lcom/labs/proj/bitmap/arrowup.bmp"); 
    Bitmap * arrowright = loadBitmap("/home/lcom/labs/proj/bitmap/arrowright.bmp"); 
    Bitmap * arrowdown = loadBitmap("/home/lcom/labs/proj/bitmap/arrowdown.bmp"); 
    Bitmap * arrowleft = loadBitmap("/home/lcom/labs/proj/bitmap/arrowleft.bmp"); 
    Bitmap * cromossoma1 = loadBitmap("/home/lcom/labs/proj/bitmap/cromossoma1.bmp"); 
    Bitmap * cromossomaup = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaup.bmp"); 
    drawBitmap(cromossoma1, 412, 20, ALIGN_LEFT);
    Bitmap * perfect = loadBitmap("/home/lcom/labs/proj/bitmap/perfect.bmp"); 
    Bitmap * great = loadBitmap("/home/lcom/labs/proj/bitmap/great.bmp"); 
    Bitmap * okay = loadBitmap("/home/lcom/labs/proj/bitmap/okay.bmp");
    Bitmap * miss = loadBitmap("/home/lcom/labs/proj/bitmap/miss.bmp"); 

    uint8_t bit_no_timer;

    if (timer_subscribe_int(&bit_no_timer) != OK)
    {
        return 1;
    }

    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_timer = BIT(bit_no_timer);

    uint8_t byte1[1], byte2[2];
    int ipc_status, r, size = 1;
    bool esc = true, make = true, wait = false;
    message msg;

    while(esc)
    {  
        /* Get a request message. */ 
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
        {
        printf("driver_receive failed with: %d", r); 
        continue;
        }
        if (is_ipc_notify(ipc_status))
        { /* received notification */ 
        switch (_ENDPOINT_P(msg.m_source))
        {
            case HARDWARE: /* hardware interrupt notification */ 
            if (msg.m_notify.interrupts & irq_set_timer) 
            {
                timer_int_handler();
            }

            if (msg.m_notify.interrupts & irq_set_keyboard) 
            { /* subscribed interrupt */ 

                kbc_ih();
            
                if (status == MSB)
                {
                wait = true;
                continue;
                } 

                if (wait == true)
                {
                wait = false;
                size = 2;
                }

                if (status == ESC_BK)
                {
                esc = false;
                make = false;
                }

                keyboardArrows(cromossomaup, pad, background, cromossoma1, okay, miss, perfect, great);
                //double_buffer_to_video_mem();

                if ((status >> 7) == BIT(0))
                {
                make = false;
                }

                if (size == 1)
                {
                byte1[0] = status;
                } 

                if (size == 2)
                {
                byte2[0] = MSB;
                byte2[1] = status;
                }
            }
                break;
                default:
            break; /* no other notifications expected: do nothing */ 
        } 
        } 
        else
        { /* received a standard message, not a notification */ 
        /* no standard messages expected: do nothing */ 
        }

        size = 1;
        make = true;

        if(!keep)
        {
        sleep(1);
        arrowRate();     
        y = 768;
        keep = true;
        continue;
        }
        else
        {
        switch(arrow)
        {
            case 0:
            {
                right = true;
                pix_map_move_pos(pad, background, arrowright, cromossoma1, 393, speed, fr_rate);
                break;
            }
            case 1:
            {
                up = true;
                pix_map_move_pos(pad, background, arrowup, cromossoma1, 393, speed, fr_rate);
                break;
            }
            case 2:
            {
                down = true;
                pix_map_move_pos(pad, background, arrowdown,    cromossoma1, 393, speed, fr_rate);
                break;
            }
            case 3:
            {
                left = true;
                pix_map_move_pos(pad, background, arrowleft, cromossoma1, 393, speed, fr_rate);
                break;
            }
        }
        }
    } 

    if (kbd_unsubscribe_int() != OK)
    {  
        return 1;
    }

    if (timer_unsubscribe_int() != OK)
    {
        return 1;
    }

    return 0;
}
