#include <lcom/lcf.h>
#include <machine/int86.h>
#include "interface.h"
#include "i8042.h"
#include "i8254.h"

#include <math.h>
#include <stdint.h>
#include <minix/sysutil.h> 


//VARIABLES
static int res_y, res_x, bits_pixel;
static char* video_mem;
static int st;
bool error = false;
uint16_t savedx, savedy;
int wd, hg, new_counter = 0;
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

  reg.u.w.ax = 0x4f02; // VBE call, function 02 -- set VBE mode
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


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) //verificar se coordenadas existem
{
	int number_pix = bits_pixel / 8;
	if(bits_pixel == 15)
  {
    number_pix = 2;
  }
	
	for(int i = 0; i < len; x++, i++)
	{
		uint32_t colortemp = color;
		int video_mem_temp = 0;

		for(int j = 0; j < number_pix; ++j, video_mem_temp++)
		{
			*(video_mem + ((y * bits_pixel * res_x) / 8 + (x * bits_pixel) / 8 )  + video_mem_temp) = colortemp;
			colortemp = colortemp >> 8;
		}
	}

	return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
	
	for(unsigned int i = 0; i < height; y++, i++)
  	{
    	vg_draw_hline(x, y, width, color);
  	}

	return 0;
}


int color_mode(unsigned int i, unsigned int j, uint8_t no_rectangles, uint32_t first, uint8_t step, uint32_t mode)
{ 
	uint32_t R,G,B;

	switch(mode)
	{
       case INDEXED:
          st = (first + (i * no_rectangles + j) * step) % (1 << bits_pixel);
          break;

        case DC_24:
          R = (((first >> 16) & 0x00FF) + j * step) % (1 << redMask);

          G = (((first >> 8) & 0x00FF) + i * step) % (1 << greenMask);

          B = ((first & 0x00FF) + (i + j) * step) % (1 << blueMask);

          st = ((R << 16) | (G << 8) | B);
          break;

           case DC_16:
          R = (((first >> 11) & 0x001F) + j * step) % (1 << redMask);

          G = (((first >> 5) & 0x003F) + i * step) % (1 << greenMask);

          B = ((first & 0x001F) + (i + j) * step) % (1 << blueMask);

          st = ((R << 11) | (G << 5) | B);
          break;

        case DC_15:
          R = (((first >> 10) & 0x001F) + j * step) % (1 << redMask);

          G = (((first >> 5) & 0x001F) + i * step) % (1 << greenMask);

          B = ((first & 0x001F) + (i + j) * step) % (1 << blueMask);

          st = ((R << 10) | (G << 5) | B);
          break;

        case DC_32:
          R = (((first >> 16) & 0x00FF) + j * step) % (1 << redMask);

          G = (((first >> 8) & 0x00FF) + i * step) % (1 << greenMask);

          B = ((first & 0x00FF) + (i + j) * step) % (1 << blueMask);

          st = ((R << 16) | (G << 8) | B);
          break;
        default:
          break;
      }

      return st;
}


void draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step, uint32_t mode)
{
	int sidex;
	int sidey;

	sidey = res_y / no_rectangles;
	sidex = res_x / no_rectangles;

	for(unsigned int y = 0, i = 0; i < no_rectangles; i++, y += sidey)
	{
		for(unsigned int x = 0, j = 0; j < no_rectangles; j++, x += sidex)
		{
			st = color_mode(i, j, no_rectangles, first, step, mode);

			vg_draw_rectangle(x, y, sidex, sidey, st);
		}
	}
}


void pix_map(const char *xpm[], uint16_t x, uint16_t y)
{
	char *sprite = read_xpm(xpm, &wd, &hg); //return null on error
	uint16_t x_ant = x;
	uint16_t y_ant = y;

	for(int i = 0; i < hg; i++, y_ant++)
	{
		if((y + i) >= res_y)
		{
			continue;
		}

		for(int j = 0; j < wd; j++, x_ant++)
		{
			if((x + j) >= res_x)
			{
				continue;
			}

			video_mem[(y_ant * bits_pixel * res_x) / 8 + (x_ant * bits_pixel) / 8] = *(sprite + i * wd + j);
		}

		x_ant = x;
	}
}


int pix_map_move_pos(const char *xpm[], uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate)
{
	if(xf > res_x || yf > res_y)
  	{
    	return 1;
  	}
	
    if(!(timer_counter % ((uint8_t) sys_hz() / fr_rate)))
   	{  
   		float dist = (fr_rate * speed) / (uint8_t) sys_hz();

    	if(y == yf)
    	{
    		if(x <= xf)
    		{
    			vg_draw_rectangle(x-dist, y, dist, hg, 0);
     	 		pix_map(xpm, x, y);
     	 		x += dist;
     	 	}

     	 	if(x > xf)
     	 	{
     	 		vg_draw_rectangle(x+wd, y, dist, hg, 0);
     	 		pix_map(xpm, x, y);
     	 		x -= dist;
     	 	}
     	}
     	else if(x == xf)
     	{
     		if(y <= yf)
    		{
    			vg_draw_rectangle(x, y-dist, wd, dist, 0);
     	 		pix_map(xpm, x, y);
     	 		y += dist;
     	 	}

     	 	if(y > yf)
     	 	{
     	 		vg_draw_rectangle(x, y+hg, wd, dist, 0);
     	 		pix_map(xpm, x, y);
     	 		y -= dist;
     	 	}
     	}
    }
	
	return 0;
}


int pix_map_move_neg(const char *xpm[], uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate)
{
	if(xf > res_x || yf > res_y)
  	{
    	return 1;
  	}
	
	if(!(timer_counter % ((uint8_t) sys_hz() / fr_rate)))
	{
		new_counter++;
	}

    if(!(new_counter % abs(speed)))
   	{
    	if(y == yf)
    	{
    		if(x <= xf)
    		{
    			vg_draw_rectangle(x - 1, y, 1, hg, 0);
     	 		pix_map(xpm, x, y);
     	 		x += 1;
     	 	}

     	 	if(x > xf)
     	 	{
     	 		vg_draw_rectangle(x + wd, y, 1, hg, 0);
     	 		pix_map(xpm, x, y);
     	 		x -= 1;
     	 	}
     	}
     	else if(x == xf)
     	{
     		if(y <= yf)
    		{
    			vg_draw_rectangle(x, y - 1, wd, 1, 0);
     	 		pix_map(xpm, x, y);
     	 		y += 1;
     	 	}

     	 	if(y > yf)
     	 	{
     	 		vg_draw_rectangle(x, y+hg, wd, 1, 0);
     	 		pix_map(xpm, x, y);
     	 		y -= 1;
     	 	}
     	}
    }

	return 0;
}
