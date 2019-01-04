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

void print_queue();

int pix_map_move_pos(int x);

int arrowRate(int i);

void keyboardArrows();

int game(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);

int gameMultiX(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart);

int gameMultiY(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse, uint8_t bit_no_uart);

void arrowProcessing(int x);

void printDance();

void powerUps(int xi, int yi, int yf);

int powerSpeed(int xi, int xf);

void changeDirect();

void reset_powerup();

void reset_game();

void end_game();

void set_current_player_cromossoma(int cr);
