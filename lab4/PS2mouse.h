#pragma once

//global variables
extern uint32_t status; //global variable that contains the scan of the buffer
extern uint32_t array[3];
extern struct packet pp;
extern bool error;
extern int timer_counter;
extern struct mouse_ev gest;
typedef enum {INIT, DRAWL,HOLD, DRAWR, FINAL} state_t;
extern state_t state;

//prototypes
int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

void (mouse_ih)(void);

void packet_create();

int mouse_write_int();

bool check_call();

int disable_int();

int disable_poll();

int mouse_poll_cmd();

int set_mouse(uint8_t write);

void check_v_line(uint8_t x_len);

bool validMoveL(uint8_t tolerance);

bool validMoveR(uint8_t tolerance);

void create_enum(uint8_t tolerance);

bool validMoveC(uint8_t tolerance);
