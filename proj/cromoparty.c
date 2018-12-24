#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"
#include "i8254.h"
#include "cromoparty.h"
#include "keyboard.h"
#include "interface.h"
#include "PS2mouse.h"


//VARIABLE INITIALIZATION
uint8_t fr_rate = 60;
int scoreCounter = 0;
Arrow **arrows;
size_t arrowsSize = 2;
int scoreprint = 0;
int cromossomaDance = 4;
int count = 0;
int colision = 0;
bool powerup = true;
int powerx = 0;
int powery = 0;
int xi =0;
int yi=0;
int yf= 300;

//FUNCTIONS
//////////////////////////////////////////////////////////////////

void init_arrows()
{
    arrows = (Arrow**)malloc(2 * sizeof(Arrow*));

    for (unsigned int i = 0; i < arrowsSize; i++)
    {
        arrows[i] = (Arrow*)malloc(sizeof (Arrow));
        arrows[i]->currentX = 0;
        arrows[i]->active = true;
        arrows[i]->speed = 0;
        arrows[i]->direction = 0;
    }

    arrows[1]->direction = 3;
    arrows[0]->speed = 3;
    arrows[1]->speed = 8;
}

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

int pix_map_move_pos(Bitmap * pad, Bitmap * background, Bitmap * arrowright, Bitmap * arrowleft, Bitmap * arrowup, Bitmap * arrowdown, uint16_t xf, Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great, Bitmap * cromossoma1, Bitmap * cromossomaup, Bitmap * cromossomadown, Bitmap * cromossomaright, Bitmap * cromossomaleft, Bitmap * pointer, Bitmap *power)
{
    if (timer_counter % (sys_hz() / fr_rate) == 0)
    {
        drawBitmap(background, 0, 0, ALIGN_LEFT);
        drawBitmap(pad, 462, 450, ALIGN_LEFT);
        changeDirect(power);
        drawBitmap(pointer, mouseX, mouseY, ALIGN_LEFT);

        for (unsigned int i = 0; i < arrowsSize; i++)
        {
            if (arrows[i]->currentX >= xf)
            {
                scoreprint = 4;
                //drawBitmap(cromossoma1, 650, 300, ALIGN_LEFT);
                //double_buffer_to_video_mem();
                arrows[i]->active = false;
            }
            else
            {
                arrows[i]->currentX += arrows[i]->speed;
            }

            switch (arrows[i]->direction)
            {
                case 0:
                {
                    drawBitmap(arrowup, arrows[i]->currentX, 450, ALIGN_LEFT);
                    break;
                }
                case 1:
                {
                    drawBitmap(arrowleft, arrows[i]->currentX, 450, ALIGN_LEFT);
                    break;
                }
                case 2:
                {
                    drawBitmap(arrowdown, arrows[i]->currentX, 450, ALIGN_LEFT);
                    break;
                }
                case 3:
                {
                    drawBitmap(arrowright, arrows[i]->currentX, 450, ALIGN_LEFT);
                    break;
                }
            }
        }  
    }
    
    printScore(okay, miss, perfect, great);
    printDance(cromossoma1, cromossomaup, cromossomadown, cromossomaright,
    cromossomaleft);
    double_buffer_to_video_mem();
   
    return 0;
}

//////////////////////////////////////////////////////////////////

int arrowRate(int i) 
{
    arrows[i]->speed = 2 + rand() % 4;  

    arrows[i]->direction = rand() % 4;

    return 0;
}

//////////////////////////////////////////////////////////////////

void keyboardArrows(Bitmap * UNUSED(pad), Bitmap * UNUSED(background))
{
    int i;

    if (abs(arrows[0]->currentX - 462) < abs(arrows[1]->currentX - 462))
    {
        i = 0;
    }
    else
    {
        i = 1;
    }

    if (status == W_KEY && arrows[i]->direction == 0)
    {
        arrows[i]->active = false;
        cromossomaDance = 0;
        score(i);       
    }
    else if (status == A_KEY && arrows[i]->direction == 1)
    {
        arrows[i]->active = false;
        cromossomaDance = 1;
        score(i);
    }
    else if (status == S_KEY && arrows[i]->direction == 2)
    {
        arrows[i]->active = false;
        cromossomaDance = 2;
        score(i);
    } 
    else if (status == D_KEY && arrows[i]->direction == 3)
    {
        arrows[i]->active = false;
        cromossomaDance = 3;
        score(i);
    }
    else
    {
        cromossomaDance = 4;
        arrows[i]->active = false;
    }
}

//////////////////////////////////////////////////////////////////

void score(int i)
{
    if (abs(462 - arrows[i]->currentX) < 10)
    {
        scoreCounter += 30;
        scoreprint = 1;
        return;
    }
    else if (abs(462 - arrows[i]->currentX) < 35)
    {
        scoreCounter += 10;
        scoreprint = 2;
        return;
    }
    else if (abs(462 - arrows[i]->currentX) < 70)
    {
        scoreCounter += 5;
        scoreprint = 3;
        return;
    }
    else
    {
        scoreprint = 4;
        return;
    }
}

//////////////////////////////////////////////////////////////////

void printScore(Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great)
{   
    switch (scoreprint)
    {
        case 1:
        {
            drawBitmap(perfect, 362, 330, ALIGN_LEFT);
            break;
        }
        case 2:
        {
            drawBitmap(great, 362, 330, ALIGN_LEFT);
            break;
        }
        case 3:
        {
            drawBitmap(okay, 362, 330, ALIGN_LEFT);
            break;
        }
        case 4:
        {
            drawBitmap(miss, 362, 330, ALIGN_LEFT);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////

void printDance(Bitmap * cromossoma1, Bitmap * cromossomaup, Bitmap * cromossomadown, Bitmap * cromossomaright, Bitmap * cromossomaleft)
{
    switch (cromossomaDance)
    {
        case 0:
        {
            drawBitmap(cromossomaup, 650, 300, ALIGN_LEFT);
            break;
        }
        case 1:
        {
            drawBitmap(cromossomaright, 650, 300, ALIGN_LEFT);
            break;
        }
        case 2:
        {
            drawBitmap(cromossomadown , 650, 300, ALIGN_LEFT);
            break;
        }
        case 3:
        {
            drawBitmap(cromossomaleft, 650, 300, ALIGN_LEFT);
            break;
        }
        case 4:
        {
            drawBitmap(cromossoma1, 650, 300, ALIGN_LEFT);
            break;
        }
    }

}

///////////////////////////////////////////////////////////////////

int powerSpeed(int xi, int xf)
{
    int speedx = abs(xf-xi)/100;

    return speedx;
}

void changeDirect(Bitmap *power)
{
    if(powerup)
    {
        powerUps(power, xi, yi, yf);

    }
    else
    {
        yf = rand() % 668;
        xi = powerx;
        yi = powery;
        colision++;
        powerup=true;
    }
}

void powerUps(Bitmap *power, int xi, int yi, int yf)
{
    int xf;
    if(xi == 0)
    {
        xf = 924;
    }
    else
    {
        xf = 0;
    }

    int speedx = powerSpeed(xi,xf);
    int speedy = powerSpeed(yi,yf);

    if(xi > xf)
    {
        speedx *= -1;
    }

    if(yi> yf)
    {
        speedy *= -1;
    }
   
    
    if ((powerx + speedx) >= xf)
    {
       powerx = xf;    
       powerup = false; 
    }
    else
    {
        powerx += speedx;
    }
    if(powery + speedy > yf) 
    {
        powery = yf;
        powerup = false; 
    }
    else
    {
        powery += speedy;
    }
    
    drawBitmap(power, powerx, powery, ALIGN_LEFT);
    
}

//////////////////////////////////////////////////////////////////

int game(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse)
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
    Bitmap *power = loadBitmap("/home/lcom/labs/proj/bitmap/power.bmp");
    Bitmap *pointer = loadBitmap("/home/lcom/labs/proj/bitmap/pointer.bmp");
    Bitmap *cromossomaleft = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaleft.bmp");
    Bitmap *cromossomaright = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaright.bmp");
    Bitmap *cromossomaup = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaup.bmp");
    drawBitmap(cromossoma1, 650, 300, ALIGN_LEFT);
    Bitmap *perfect = loadBitmap("/home/lcom/labs/proj/bitmap/perfect.bmp");
    Bitmap *great = loadBitmap("/home/lcom/labs/proj/bitmap/great.bmp");
    Bitmap *okay = loadBitmap("/home/lcom/labs/proj/bitmap/okay.bmp");
    Bitmap *miss = loadBitmap("/home/lcom/labs/proj/bitmap/miss.bmp");

    uint32_t irq_set_timer = BIT(bit_no_timer);
    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_mouse = BIT(bit_no_mouse);
    
    uint8_t byte1[1], byte2[2];
    int ipc_status, r, size = 1, s = 1;
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
                        }

                        if (status == W_KEY || status == A_KEY || status == S_KEY ||status  == D_KEY)
                        {
                            keyboardArrows(pad, background);
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

                    if (msg.m_notify.interrupts & irq_set_mouse)
                    { /* subscribed interrupt */

                        mouse_ih();

                        if (s == 1)
                        {
                            if (status_mouse & BIT(3))
                            {
                                pp.bytes[0] = status_mouse;
                                s++;
                            }
                            continue;
                        } 

                        if (s == 2)
                        {
                            pp.bytes[1] = status_mouse;
                            s++;
                            continue;
                        }
                        
                        if (s == 3)
                        {
                            pp.bytes[2] = status_mouse;
                            packet_create();
                            currentMousePosition();
                            s = 1;
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
        arrowProcessing(cromossoma1, pad, background, arrowdown, arrowup, arrowleft, arrowright, okay, miss, perfect, great, cromossomaup, cromossomadown, cromossomaright,  cromossomaleft, pointer, power);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////

void arrowProcessing(Bitmap * cromossoma1, Bitmap * pad, Bitmap * background, Bitmap * arrowdown, Bitmap * arrowup, Bitmap * arrowleft, Bitmap * arrowright, Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great, Bitmap * cromossomaup, Bitmap * cromossomadown, Bitmap * cromossomaright, Bitmap * cromossomaleft,  Bitmap * pointer, Bitmap *power)
{
    if (!arrows[0]->active)
    {
        arrowRate(0);
        arrows[0]->currentX = 0;
        arrows[0]->active = true;
        return;
    }
    else if (!arrows[1]->active)
    {
        arrowRate(1);
        arrows[1]->currentX = 0;
        arrows[1]->active = true;
        return;
    }
    else
    {
        pix_map_move_pos(pad, background, arrowright, arrowleft, arrowup, arrowdown, 1024, okay, miss, perfect, great, cromossoma1, cromossomaup, cromossomadown, cromossomaright, cromossomaleft, pointer,power);
    }
}

