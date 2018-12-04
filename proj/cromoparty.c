#include <machine/int86.h>
#include <lcom/lcf.h>
#include "cromoparty.h"

#include <math.h>
#include <stdint.h>
#include <minix/sysutil.h> 

static int res_y, res_x;
static unsigned bits_pixel;
static char* video_mem;
uint8_t blueMask, greenMask, redMask;

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

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
  panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  /* Map memory */
    
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if(video_mem == MAP_FAILED)
    panic("couldn’t map video memory");

  //VARIABLES
  struct reg86u reg;
  memset(&reg, 0, sizeof(reg));

  reg.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
  reg.u.w.bx = (1 << 14) | mode; // set bit 14: linear framebuffer
  reg.u.b.intno = 0x10;
        
  if(sys_int86(&reg) != OK ) 
  {
    printf("set_vbe_mode: sys_int86() failed \n");
    return NULL;
  }

  return video_mem;
}


int get_mode_info(uint16_t mode, vbe_mode_info_t * vmi_p) 
{
  struct reg86u reg;
  memset(&reg, 0, sizeof(reg));
  mmap_t m;
  lm_init(true);
  if(lm_alloc(sizeof(vbe_mode_info_t), &m) == NULL)
  {
    return 1;
  }
  
  reg.u.w.ax = 0x4F01;
  reg.u.w.cx = mode;
  reg.u.b.intno = 0x10;
  reg.u.w.es = PB2BASE(m.phys);
  reg.u.w.di = PB2OFF(m.phys);
  if(sys_int86(&reg) != OK )
  {
    printf("set_vbe_mode: sys_int86() failed \n");
    lm_free(&m);
    return 1;
  }
  if(reg.u.b.ah != 0)
  {
    lm_free(&m);
    return 1;
  }
  *vmi_p = *((vbe_mode_info_t*) m.virt);
  lm_free(&m);
  return 0;
}

Bitmap* loadBitmap(const char* filename) {
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
    if (bitmapFileHeader.type != 0x4D42) {
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

    if (rd != 1) {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    unsigned char* bitmapImage = (unsigned char*) malloc(
            bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;

    return bmp;
}

void setPixel(int color, uint16_t x_ant, uint16_t y_ant)
{
  video_mem[(y_ant * bits_pixel * res_x) / 8 + (x_ant * bits_pixel) / 8] = color;
}

void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment) {
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > res_x || y + height < 0
            || y > res_y)
        return;

    int xCorrection = 0;
    if (x < 0) {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > res_x)
            drawWidth = res_x;
    } else if (x + drawWidth >= res_x) {
        drawWidth = res_x - x;
    }

    char* bufferStartPos;
    char* imgStartPos;

    int i;
    for (i = 0; i < height; i++) {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= res_y)
            continue;

        bufferStartPos = video_mem;
        bufferStartPos += x * 4 + pos * res_x * 4;

        imgStartPos = (char*)(bmp->bitmapData) + xCorrection * 4 + i * width * 4;

        memcpy(bufferStartPos, imgStartPos, drawWidth * 4);
    }
}

// void drawBitmap(Bitmap* bmp, int x, int y) {
//     if (bmp == NULL)
//         return;

//     uint16_t x_ant = x;
//     uint16_t y_ant = y;
//     int colorCounter = 0;

//      for(int i = 0; i < y; i++, y_ant++)
//       {
//          if((y + i) >= res_y)
//           {
//             continue;
//           }
//          for(int j = 0; j < x; j++, x_ant++, colorCounter += 4)
//           {
//              if((x + j) >= res_x)
//              {
//                  continue;
//              }

//               setPixel(bmp->bitmapData[colorCounter], x_ant, y_ant);
//           }

//        x_ant = x;
         
//     }
// }

void deleteBitmap(Bitmap* bmp) {
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}
