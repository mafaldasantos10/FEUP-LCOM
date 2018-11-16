#include <lcom/lcf.h>
#include "interface.h"
#include <machine/int86.h>
#include <math.h>

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"

static int res_y,res_x,pix_byte;

void *(vg_init)(uint16_t mode)
{//fazer as 2 funcoes ha parte (statics)

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

vbe_mode_info_t vmi_p;

vbe_get_mode_info (mode, &vmi_p);

pix_byte = vmi_p.BitsPerPixel;
res_y = vmi_p.YResolution;
res_x = vmi_p.XResolution;

int r;
struct minix_mem_range mr; /* physical memory range */
unsigned int vram_base = vmi_p.PhysBasePtr; /* VRAM’s physical addresss */
unsigned int vram_size = res_y*res_x*ceil(pix_byte/8); /* VRAM’s size, but you can use
the frame-buffer size, instead */
void *video_mem; /* frame-buffer VM address */
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
