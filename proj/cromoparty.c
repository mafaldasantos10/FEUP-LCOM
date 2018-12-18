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
uint8_t fr_rate = 60;
int scoreCounter = 0;
Arrow **arrows;
size_t arrowsSize = 2;

void init_arrows()
{
arrows = (Arrow**)malloc(2 * sizeof(Arrow*));

    for(unsigned int i=0;i<arrowsSize; i++)
    {
        arrows[i]=(Arrow*)malloc(sizeof (Arrow));
        arrows[i]->currentX = 0;
        arrows[i]->active = true;
        arrows[i]->speed = 0;
    }
        arrows[0]->speed = 3;
        arrows[1]->speed = 8;
}


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

int pix_map_move_pos(Bitmap * pad, Bitmap * background, Bitmap * arrow, Bitmap * cromossoma1, Bitmap * miss, uint16_t xf, int i)
{
    if (timer_counter % (sys_hz() / fr_rate) == 0)
    {
        drawBitmap(background, 0, 0, ALIGN_LEFT);
        drawBitmap(pad, 462, 450, ALIGN_LEFT);  

        if (arrows[i]->currentX >= xf)
        {
            drawBitmap(miss, 362, 330, ALIGN_LEFT);
            drawBitmap(cromossoma1, 650, 300, ALIGN_LEFT);
            double_buffer_to_video_mem();
            sleep(1);
            arrows[i]->active = false;
        }
        else
        {
            arrows[i]->currentX += arrows[i]->speed;
        }

        drawBitmap(arrow, arrows[i]->currentX, 450, ALIGN_LEFT);
        drawBitmap(cromossoma1, 650, 300, ALIGN_LEFT);
        double_buffer_to_video_mem();
    }
   
    return 0;
}

//////////////////////////////////////////////////////////////////

int arrowRate() 
{
    for(unsigned int i=0;i<arrowsSize;i++)
    {
    	arrows[i]->speed = 5 + rand() % 6;
    }

    for(unsigned int i=0;i<arrowsSize;i++)
    {
        arrows[i]->direction = rand() % 4;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////

void keyboardArrows(Bitmap * cromossomaup, Bitmap * pad, Bitmap * background,  Bitmap * cromossoma1, Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great, Bitmap * cromossomadown, Bitmap * cromossomaright, Bitmap * cromossomaleft)
{
    drawBitmap(background, 0, 0, ALIGN_LEFT);
    drawBitmap(pad, 462, 450, ALIGN_LEFT);
    double_buffer_to_video_mem();

    for(unsigned int i =0; i<arrowsSize;i++)
    {

        if (status == W_KEY && arrows[i]->direction == 1)
        {
                arrows[i]->active = false;
                drawBitmap(cromossomaup, 650, 300, ALIGN_LEFT);
                score(okay, miss, perfect, great, i);
                double_buffer_to_video_mem();
                sleep(1);          
        }

        else if (status == D_KEY && arrows[i]->direction == 0)
        {
                arrows[i]->active = false;
                drawBitmap(cromossomaright, 650, 300, ALIGN_LEFT);
                score(okay, miss, perfect, great, i);
                double_buffer_to_video_mem();
                sleep(1);
        }

        else if (status == S_KEY && arrows[i]->direction == 2)
        {
                arrows[i]->active = false;
                drawBitmap(cromossomadown, 650, 300, ALIGN_LEFT);
                score(okay, miss, perfect, great, i);
                double_buffer_to_video_mem();
                sleep(1);

        } 

        else if (status == A_KEY && arrows[i]->direction == 3)
        {
                arrows[i]->active = false;
                drawBitmap(cromossomaleft, 650, 300, ALIGN_LEFT);
                score(okay, miss, perfect, great, i);
                double_buffer_to_video_mem();
                sleep(1);
        }

        else
        {
            arrows[i]->active = false;
            drawBitmap(cromossoma1, 650, 300, ALIGN_LEFT);
            drawBitmap(miss, 362, 330, ALIGN_LEFT);
            double_buffer_to_video_mem();
            sleep(1);
        }

        drawBitmap(background, 0, 0, ALIGN_LEFT);
        drawBitmap(pad, 462, 450, ALIGN_LEFT);
        drawBitmap(cromossoma1, 650, 300, ALIGN_LEFT);
        double_buffer_to_video_mem();
    }
}

//////////////////////////////////////////////////////////////////

void score(Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great, int i)
{
    if (abs(462 - arrows[i]->currentX) < 10)
    {
        drawBitmap(perfect, 362, 330, ALIGN_LEFT);
        scoreCounter += 30;
        return;
    }
    else if (abs(462 - arrows[i]->currentX) < 35)
    {
        drawBitmap(great, 362, 330, ALIGN_LEFT);
        scoreCounter += 10;
        return;
    }
    else if (abs(462 - arrows[i]->currentX) < 70)
    {
        drawBitmap(okay, 362, 330, ALIGN_LEFT);
        scoreCounter += 5;
        return;
    }
    else
    {
        drawBitmap(miss, 362, 330, ALIGN_LEFT);
        return;
    }
}

int game(uint8_t bit_no_kb)
{
init_arrows();

    Bitmap *background = loadBitmap("/home/lcom/labs/proj/bitmap/discof.bmp");
    drawBitmap(background, 0, 0, ALIGN_LEFT);
    Bitmap *pad = loadBitmap("/home/lcom/labs/proj/bitmap/pad.bmp");
    drawBitmap(pad, 462, 450, ALIGN_LEFT);
    Bitmap *arrowup = loadBitmap("/home/lcom/labs/proj/bitmap/arrowup.bmp");
    Bitmap *arrowright = loadBitmap("/home/lcom/labs/proj/bitmap/arrowright.bmp");
    Bitmap *arrowdown = loadBitmap("/home/lcom/labs/proj/bitmap/arrowdown.bmp");
    Bitmap *arrowleft = loadBitmap("/home/lcom/labs/proj/bitmap/arrowleft.bmp");
    Bitmap *cromossoma1 = loadBitmap("/home/lcom/labs/proj/bitmap/cromossoma1.bmp");
    Bitmap *cromossomadown = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomadown.bmp");
    Bitmap *cromossomaleft = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaleft.bmp");
    Bitmap *cromossomaright = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaright.bmp");
    Bitmap *cromossomaup = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaup.bmp");
    drawBitmap(cromossoma1, 650, 300, ALIGN_LEFT);
    Bitmap *perfect = loadBitmap("/home/lcom/labs/proj/bitmap/perfect.bmp");
    Bitmap *great = loadBitmap("/home/lcom/labs/proj/bitmap/great.bmp");
    Bitmap *okay = loadBitmap("/home/lcom/labs/proj/bitmap/okay.bmp");
    Bitmap *miss = loadBitmap("/home/lcom/labs/proj/bitmap/miss.bmp");

    uint8_t bit_no_timer;

    if (timer_subscribe_int(&bit_no_timer) != OK)
    {
        return 1;
    }

    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_timer = BIT(bit_no_timer);

    uint8_t byte1[1], byte2[2];
    int ipc_status, r, size = 1;
    bool esc = true, wait = false;
    message msg;

    while (esc)
    {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status))
        { /* received notification */
        switch (_ENDPOINT_P(msg.m_source))
        {
            case HARDWARE: /* hardware interrupt notification */
                if (msg.m_notify.interrupts & irq_set_timer) {
                    timer_int_handler();
                }

                if (msg.m_notify.interrupts & irq_set_keyboard) { /* subscribed interrupt */

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
                    }

                    if (status == W_KEY || status == A_KEY || status == S_KEY || status == D_KEY)
                    {
                        keyboardArrows(cromossomaup, pad, background, cromossoma1, okay, miss, perfect, great, cromossomadown, cromossomaright, cromossomaleft);
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
        {   /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }

        size = 1;

        arrowProcessing(cromossoma1, pad, background, arrowdown, arrowup, arrowleft, arrowright,miss);
    }

    if (timer_unsubscribe_int() != OK)
    {
        return 1;
    }

  return 0;
}

void arrowProcessing(Bitmap * cromossoma1, Bitmap * pad, Bitmap * background, Bitmap * arrowdown, Bitmap * arrowup, Bitmap * arrowleft, Bitmap * arrowright, Bitmap * miss)
{
        if (!arrows[0]->active)
        {
            sleep(1);
            arrowRate();
            arrows[0]->currentX = 0;
            arrows[0]->active = true;
            return;
        }
        else if(!arrows[1]->active)
        {
            sleep(1);
            arrowRate();
            arrows[1]->currentX = 0;
            arrows[1]->active = true;
            return;
        }
        else
        {
            for(unsigned int i = 0; i<arrowsSize;i++)
            {
                switch (arrows[i]->direction)
                {
                    case 0:
                    {
                        pix_map_move_pos(pad, background, arrowright, cromossoma1, miss, 1024, i);
                        break;
                    }
                    case 1:
                    {
                        pix_map_move_pos(pad, background, arrowup, cromossoma1, miss, 1024, i);
                        break;
                    }
                    case 2:
                    {
                        pix_map_move_pos(pad, background, arrowdown, cromossoma1, miss, 1024, i);
                        break;
                    }
                    case 3:
                    {
                        pix_map_move_pos(pad, background, arrowleft, cromossoma1, miss, 1024, i);
                        break;
                    }
                }
            }
        }
}
