#include <lcom/lcf.h>
#include <machine/int86.h>
#include "interface.h"
#include "i8042.h"

#include <math.h>
#include <stdint.h>
#include <minix/sysutil.h> 

static int res_y, res_x, bits_pixel;
static char* video_mem;
bool error = false;


void *(vg_init)(uint16_t mode)
{
	//VARIABLES
	struct reg86u reg;
memset(&reg, 0, sizeof(reg));
        reg.u.w.ax = 0x4f02; // VBE call, function 02 -- set VBE mode
        reg.u.w.bx = (1<<14)|mode; // set bit 14: linear framebuffer
        reg.u.b.intno = 0x10;
if(sys_int86(&reg) != OK ) 
        {
        printf("set_vbe_mode: sys_int86() failed \n");
        return NULL;
        }
        lm_init(true);
vbe_mode_info_t vmi_p;
vbe_get_mode_info (mode, &vmi_p);
bits_pixel = vmi_p.BitsPerPixel;
res_y = vmi_p.YResolution;
res_x = vmi_p.XResolution;
int r;
struct minix_mem_range mr; /* physical memory range */
unsigned int vram_base = vmi_p.PhysBasePtr; /* VRAM’s physical addresss */
unsigned int vram_size = res_y*res_x*ceil(bits_pixel/8); /* VRAM’s size, but you can use
the frame-buffer size, instead */
//void *video_mem; /* frame-buffer VM address */
/* Allow memory mapping */
mr.mr_base = (phys_bytes) vram_base;
mr.mr_limit = mr.mr_base + vram_size;
if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
panic("sys_privctl (ADD_MEM) failed: %d\n", r);
/* Map memory */
video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
if(video_mem == MAP_FAILED)
panic("couldn’t map video memory");
return video_mem;
}


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) //verificar se coordenadas existem
{
	for(unsigned int i=0; i<len;x++, i++)
	{
		video_mem[(y*bits_pixel*res_x)/8 + (x*bits_pixel)/8]=color;
	}
	return 0;
}

void draw_rectangle(uint16_t x, uint16_t y, uint16_t len, uint32_t color,uint16_t height)
{
	
for(unsigned int i=0; i<height;y++, i++)
  {
    vg_draw_hline(x,y,len,color);
  }
}

void draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step)
{
	int sidex;
	int sidey;
	static int st;


	sidey=res_y/no_rectangles;
	sidex=res_x/no_rectangles;


	//st= first;
	for(unsigned int y=0,i=0;i<no_rectangles;i++,y+=sidey)
	{
		for(unsigned int x=0,j=0;j<no_rectangles;j++,x+=sidex)
		{
			st = (first + (i * no_rectangles + j) * step) % (1 << bits_pixel);
			//st+=step;

		draw_rectangle(x, y, sidex, st, sidey);

		}
	}
}
