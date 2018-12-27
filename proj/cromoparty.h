#pragma once

//STRUCTS
//////////////////////////////////////////////////////////////////

//arrow
typedef struct {
    int currentX;
    int speed;
    bool active;
    int direction;
} Arrow;


//PROTOTYPES
//////////////////////////////////////////////////////////////////

void init_arrows();

int pix_map_move_pos();

int arrowRate(int i);

void keyboardArrows();

int game(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);

void arrowProcessing();

void printDance();

void powerUps(int xi, int yi, int yf);

int powerSpeed(int xi, int xf);

void changeDirect();
