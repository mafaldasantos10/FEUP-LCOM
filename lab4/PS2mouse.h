#pragma once

//global variables
extern uint32_t status, stat; //global variable that contains the scan of the buffer
extern uint32_t array[3];
extern struct packet pp;
extern bool error;
extern int timer_counter;
//extern uint32_t counter;
//extern int timer_counter;
//extern bool error;

//#ifdef LAB4
//int sys_inb_cnt(port_t port, uint32_t *byte);
//#else
//#define sys_inb_cnt(p,q) sys_inb(p,q) 
//#endif

//prototypes
int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

void (mouse_ih)(void);

void packet_create();

void mouse_write_int();

void disable_int();

void enable_poll();

void disable_poll();

int mouse_poll();

int mouse_poll_cmd(bool finish);

void int_mouse(uint8_t write);

void check_v_line(mouse_ev_t evt);

bool validMoveL(uint8_t tolerance);

bool validMoveR(uint8_t tolerance);
