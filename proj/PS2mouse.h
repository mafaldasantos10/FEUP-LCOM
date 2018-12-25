#pragma once

//GLOBAL VARIABLES
extern uint32_t status_mouse; //global variable that with the contents of the buffer
extern struct packet pp;
extern bool error_mouse;


//PROTOTYPES
int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

void (mouse_ih)(void);

void packet_create();

int set_mouse(uint8_t write);

int mouse_write_int();

int disable_int();

void currentMousePosition();

int get_mouseX();

int get_mouseY();
