#include <machine/int86.h>
#include "cromoparty.h"

#include <math.h>
#include <stdint.h>
#include <minix/sysutil.h> 

static int res_y, res_x;
static unsigned bits_pixel;
static char* video_mem;

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


unsigned char *LoadBitmapFile(char *filename, bitmapv5header *bitmapv5header)
{
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx = 0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType != 0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(BITMAPV5HEADER, sizeof(BITMAPINFOHEADER), 1, filePtr);

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapv5header->biSizeImage);

    //verify memory allocation
    if (!bitmapImage)
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmapImage, bitmapv5header->biSizeImage, filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0; imageIdx < bitmapv5header->biSizeImage; imageIdx += 3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    //close file and return bitmap image data
    fclose(filePtr);
    return bitmapImage;
}

void setPixel(uint16_t x, uint16_t y, int color)
{
	//32
	video_mem[(y_ant * bits_pixel * res_x) / 8 + (x_ant * bits_pixel) / 8] = color;
}
void bit_map(const char *bitmap[], uint16_t x, uint16_t y)
{
        uint16_t x_ant = x;
        uint16_t y_ant = y;
        int colorCounter = 0;

        for(int i = 0; i < hg; i++, y_ant++)
        {
                if((y + i) >= res_y)
                {
                        continue;
                }
                for(int j = 0; j < wd; j++, x_ant++,colorCounter+=4)
                {
                        if((x + j) >= res_x)
                        {
                                continue;
                        }
                       setPixel(x, y, bitmap[colorCounter]);
                }
                x_ant = x;
        }
}
