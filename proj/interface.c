#include <lcom/lcf.h>

#include <stdint.h>
#include <minix/sysutil.h> 
#include <math.h>

#include "i8042.h"
#include "i8254.h"
#include "interface.h"

//VARIABLES
int res_y, res_x;
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


    //VARIABLES
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
    //printf("cmd:\n");
    //uint32_t cmd;
    //sys_inb(INPUT_STATUS, &cmd);
    //while (cmd & VRETRACE);
    //while (!(cmd & VRETRACE));
    memcpy(video_mem, double_buffer, res_y * res_x * ceil(bits_pixel / 8));

    
    //VSync();
    //printf("cmd2:\n");
}
