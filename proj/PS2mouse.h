#pragma once

//GLOBAL VARIABLES
extern uint32_t status_mouse; //global variable that contains the scan of the buffer
extern struct packet pp;
extern bool error_mouse;
extern int mouseX;
extern int mouseY;


//PROTOTYPES
int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

void (mouse_ih)(void);

void packet_create();

int mouse_write_int();

int disable_int();

int set_mouse(uint8_t write);

void currentMousePosition();
