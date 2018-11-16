#include <lcom/lcf.h>
#include <machine/int86.h>
#include "interface.h"
#include "i8042.h"

#include <math.h>
#include <stdint.h>
#include <minix/sysutil.h> 

static int res_y, res_x, bits_pixel;
bool error = false;


static void * mapping()
{
	vbe_mode_info_t vmi_p;

	vbe_get_mode_info (mode, &vmi_p);

	bits_pixel = vmi_p.BitsPerPixel;
	res_y = vmi_p.YResolution;
	res_x = vmi_p.XResolution;

	//VARIABLES
	int r;
	struct minix_mem_range mr; /* physical memory range */
	unsigned int vram_base = vmi_p.PhysBasePtr; /* VRAM’s physical addresss */
	unsigned int vram_size = res_y * res_x * ceil(bits_pixel / 8); /* VRAM’s size, but you can use the frame-buffer size, instead */
	void *video_mem; /* frame-buffer VM address */

	/* Allow memory mapping */
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
	{
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);
		error = true;
	}

	/* Map memory */
	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

	if(video_mem == MAP_FAILED)
	{
		panic("couldn’t map video memory");
		error = true;
	}

	return video_mem;
}


static void call(uint16_t mode)
{
	struct reg86u reg;

	memset(&reg, 0, sizeof(reg));
	reg.u.w.ax = 0x4f02; // VBE call, function 02 -- set VBE mode
	reg.u.w.bx = (1 << 14) | mode; // set bit 14: linear framebuffer
	reg.u.b.intno = 0x10;

	if(sys_int86(&reg) != OK ) 
	{
		printf("set_vbe_mode: sys_int86() failed \n");
		error = true;
	}

	return;
}


void *(vg_init)(uint16_t mode)
{
	//VARIABLES
	void *video_mem = mapping();

	if(error)
	{
		return NULL;
	}

	error = false;

	call(mode);

	if(error)
	{
		return NULL;
	}

	return video_mem;
}
