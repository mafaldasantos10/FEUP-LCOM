#include <lcom/lcf.h>

#include <stdint.h>
#include <minix/sysutil.h> 
#include <math.h>

#include "i8042.h"
#include "i8254.h"
#include "interface.h"
#include "cromoparty.h"

//VARIABLES
int res_x, res_y;
unsigned bits_pixel;

uint8_t blueMask, greenMask, redMask;
uint8_t blueMask, greenMask, redMask;

static unsigned char* video_mem;
unsigned char* double_buffer;

//FUNCTIONS
//////////////////////////////////////////////////////////////////

void *(vg_init)(uint16_t mode)
{
    lm_init(true);
    vbe_mode_info_t vmi_p;
    get_mode_info(mode, &vmi_p);

    bits_pixel = vmi_p.BitsPerPixel;
    res_y = vmi_p.YResolution;
    res_x = vmi_p.XResolution;
    int r;

    struct minix_mem_range mr; /* physical memory range */
    unsigned int vram_base = vmi_p.PhysBasePtr; /* VRAM’s physical addresss */
    unsigned int vram_size = res_y * res_x * ceil(bits_pixel / 8); /* VRAM’s size, but you can use the frame-buffer size, instead */

    redMask = vmi_p.RedMaskSize;
    blueMask = vmi_p.BlueMaskSize;
    greenMask = vmi_p.GreenMaskSize;
    /* frame-buffer VM address */
    /* Allow memory mapping */
    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if ( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  
    /* Map memory */
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    if (video_mem == MAP_FAILED)
        panic("couldn’t map video memory");

    double_buffer = (unsigned char *) malloc(vram_size);

    if (double_buffer == NULL)
        panic("not enough memory");

    struct reg86u reg;
    memset(&reg, 0, sizeof(reg));

    reg.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
    reg.u.w.bx = (1 << 14) | mode; // set bit 14: linear framebuffer
    reg.u.b.intno = 0x10;
        
    if (sys_int86(&reg) != OK ) 
    {
        printf("set_vbe_mode: sys_int86() failed \n");
        return NULL;
    }

    return video_mem;
}

//////////////////////////////////////////////////////////////////

int get_mode_info(uint16_t mode, vbe_mode_info_t * vmi_p) 
{
    struct reg86u reg;
    memset(&reg, 0, sizeof(reg));
    mmap_t m;
    lm_init(true);

    if (lm_alloc(sizeof(vbe_mode_info_t), &m) == NULL)
    {
        return 1;
    }
  
    reg.u.w.ax = 0x4F01;
    reg.u.w.cx = mode;
    reg.u.b.intno = 0x10;
    reg.u.w.es = PB2BASE(m.phys);
    reg.u.w.di = PB2OFF(m.phys);

    if (sys_int86(&reg) != OK )
    {
        printf("set_vbe_mode: sys_int86() failed \n");
        lm_free(&m);
        return 1;
    }

    if (reg.u.b.ah != 0)
    {
        lm_free(&m);
        return 1;
    }

    *vmi_p = *((vbe_mode_info_t*) m.virt);
    lm_free(&m);

    return 0;
}

//////////////////////////////////////////////////////////////////

void double_buffer_to_video_mem() 
{
    memcpy(video_mem, double_buffer, res_y * res_x * ceil(bits_pixel / 8));
}

//////////////////////////////////////////////////////////////////

int get_horizontal_resolution()
{
	return res_x;
}

//////////////////////////////////////////////////////////////////

int get_vertical_resolution()
{
	return res_y;
}

//////////////////////////////////////////////////////////////////

Images images;

void loadImages()
{
    images.one = loadBitmap("/home/lcom/labs/proj/bitmap/1.bmp");
    images.two = loadBitmap("/home/lcom/labs/proj/bitmap/2.bmp");
    images.three = loadBitmap("/home/lcom/labs/proj/bitmap/3.bmp");
    images.four = loadBitmap("/home/lcom/labs/proj/bitmap/4.bmp");
    images.five = loadBitmap("/home/lcom/labs/proj/bitmap/5.bmp");
    images.six = loadBitmap("/home/lcom/labs/proj/bitmap/6.bmp");
    images.seven = loadBitmap("/home/lcom/labs/proj/bitmap/7.bmp");
    images.eight = loadBitmap("/home/lcom/labs/proj/bitmap/8.bmp");
    images.nine = loadBitmap("/home/lcom/labs/proj/bitmap/9.bmp");
    images.zero = loadBitmap("/home/lcom/labs/proj/bitmap/0.bmp");

    images.background = loadBitmap("/home/lcom/labs/proj/bitmap/discof.bmp");
    images.pad = loadBitmap("/home/lcom/labs/proj/bitmap/pad.bmp");
    images.arrow_up = loadBitmap("/home/lcom/labs/proj/bitmap/arrowup.bmp");
    images.arrow_left = loadBitmap("/home/lcom/labs/proj/bitmap/arrowleft.bmp");
    images.arrow_down = loadBitmap("/home/lcom/labs/proj/bitmap/arrowdown.bmp");
    images.arrow_right = loadBitmap("/home/lcom/labs/proj/bitmap/arrowright.bmp");
    images.cromossoma_idle = loadBitmap("/home/lcom/labs/proj/bitmap/cromossoma1.bmp");
    images.cromossoma_up = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaup.bmp");
    images.cromossoma_left = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaleft.bmp");
    images.cromossoma_down = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomadown.bmp");
    images.cromossoma_right = loadBitmap("/home/lcom/labs/proj/bitmap/cromossomaright.bmp");
    images.perfect = loadBitmap("/home/lcom/labs/proj/bitmap/perfect.bmp");
    images.great = loadBitmap("/home/lcom/labs/proj/bitmap/great.bmp");
    images.okay = loadBitmap("/home/lcom/labs/proj/bitmap/okay.bmp");
    images.miss = loadBitmap("/home/lcom/labs/proj/bitmap/miss.bmp");
    images.power = loadBitmap("/home/lcom/labs/proj/bitmap/power.bmp");
    images.pointer = loadBitmap("/home/lcom/labs/proj/bitmap/pointer.bmp");

    images.start_selected = loadBitmap("/home/lcom/labs/proj/bitmap/start2.bmp");
    images.instructions_selected = loadBitmap("/home/lcom/labs/proj/bitmap/instructions2.bmp");
    images.highscores_selected = loadBitmap("/home/lcom/labs/proj/bitmap/highscores2.bmp");
    images.exit_selected = loadBitmap("/home/lcom/labs/proj/bitmap/exit2.bmp");
    images.start_not_selected = loadBitmap("/home/lcom/labs/proj/bitmap/start1.bmp");
    images.instructions_not_selected = loadBitmap("/home/lcom/labs/proj/bitmap/instructions1.bmp");
    images.highscores_not_selected = loadBitmap("/home/lcom/labs/proj/bitmap/highscores1.bmp");
    images.exit_not_selected = loadBitmap("/home/lcom/labs/proj/bitmap/exit1.bmp");
    images.menu = loadBitmap("/home/lcom/labs/proj/bitmap/menu.bmp");
    images.instructions = loadBitmap("/home/lcom/labs/proj/bitmap/instructionmenu.bmp");
    images.pause = loadBitmap("/home/lcom/labs/proj/bitmap/pause.bmp");
}

//////////////////////////////////////////////////////////////////

Images images;

void deleteImages()
{
    deleteBitmap(images.one);
    deleteBitmap(images.two);
    deleteBitmap(images.three);
    deleteBitmap(images.four);
    deleteBitmap(images.five);
    deleteBitmap(images.six);
    deleteBitmap(images.seven);
    deleteBitmap(images.eight);
    deleteBitmap(images.nine);
    deleteBitmap(images.zero);
    deleteBitmap(images.background);
    deleteBitmap(images.pad);
    deleteBitmap(images.arrow_up);
    deleteBitmap(images.arrow_left);
    deleteBitmap(images.arrow_down);
    deleteBitmap(images.arrow_right);
    deleteBitmap(images.cromossoma_idle);
    deleteBitmap(images.cromossoma_up);
    deleteBitmap(images.cromossoma_left);
    deleteBitmap(images.cromossoma_down);
    deleteBitmap(images.cromossoma_right);
    deleteBitmap(images.perfect);
    deleteBitmap(images.great);
    deleteBitmap(images.okay);
    deleteBitmap(images.miss);
    deleteBitmap(images.power);
    deleteBitmap(images.pointer);  
    deleteBitmap(images.start_selected);
    deleteBitmap(images.instructions_selected);
    deleteBitmap(images.highscores_selected);
    deleteBitmap(images.exit_selected);
    deleteBitmap(images.start_not_selected);
    deleteBitmap(images.instructions_not_selected);
    deleteBitmap(images.highscores_not_selected);
    deleteBitmap(images.exit_not_selected);
    deleteBitmap(images.menu);
    deleteBitmap(images.instructions);
    deleteBitmap(images.pause);
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
    bmp->colided = false;
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

    if (x + width < 0 || x > get_horizontal_resolution() || y + height < 0 || y > get_vertical_resolution())
        return;

    int xCorrection = 0;
    if (x < 0) 
    {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > get_horizontal_resolution())
            drawWidth = get_horizontal_resolution();
    } 
    else if (x + drawWidth >= get_horizontal_resolution()) 
    {
        drawWidth = get_horizontal_resolution() - x;
    }

    unsigned char* bufferStartPos;
    unsigned char* imgStartPos;

    int i;
    for (i = 0; i < height; i++) 
    {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= get_vertical_resolution())
            continue;

        bufferStartPos = double_buffer;

        bufferStartPos += x * 4 + pos * get_horizontal_resolution() * 4;

        imgStartPos = (unsigned char*)(bmp->bitmapData) + xCorrection * 4 + i * width * 4;

        uint32_t *ptr =(uint32_t*) imgStartPos;
        uint32_t *buff =(uint32_t*) bufferStartPos;

        for(int j = 0; j < drawWidth; j ++)
        {

            if(ptr[j] != 0x1f0ff8)
            {
                    if(buff[j] == 0x630000)
                    {
                        
                        bmp->colided = true;
                    }  

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
