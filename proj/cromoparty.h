#pragma once

//STRUCTS
//////////////////////////////////////////////////////////////////

/** @brief Arrow Struct */
typedef struct {
    /** @brief Position of the arrow in the X axis */
    int currentX;
    /** @brief Speed of the arrow in pixels */
    int speed;
    /** @brief Direction of the arrow */
    int direction;
    /** @brief State of the arrow */
    bool active;
} Arrow;


//PROTOTYPES
//////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the array arrows of struct Arrow 
 */
void init_arrows();

/**
 * @brief Draws multiple things that must be on the screen everytime it is refreshed
 */
void print_queue();

/**
 * @brief Prints everything that is supposed to be on the screen everytime the screen is refreshed
 * 
 * @param x 0 if it's in single payer, 1 for for player using chromosome X, 2 for player using chromosome Y
 * @return 0 on success, non-zero otherwise
 */
int pix_map_move_pos(int x);

/**
 * @brief Changes the speed and direction of a particular arrow
 * 
 * @param i Index of the arrow in the array of Arrows to be changed
 * @return 0 on success, non-zero otherwise
 */
int arrowRate(int i);

/**
 * @brief Checks if the user pressed the right key comparing it to the closest to the center arrow direction
 */
void keyboardArrows();

/**
 * @brief Makes the chromosome execute the correct dance step according to the player's performance
 */
void printDance();

int powerSpeed(int xi, int xf);

void changeDirect();

void powerUps(int xi, int yi, int yf);

/**
 * @brief Resets all veriables related to the power up, making it ready for a new game
 */
void reset_powerup();

/**
 * @brief Resets all veriables related the game itselft, making it ready for a new game
 */
void reset_game();

/**
 * @brief Saves the score if it is a highscore, and goes to the game over menu where score is shown and after that returns to the menu
 */
void end_game();

/**
 * @brief If an arrow is inactive, it is initiated and calls for the fuction that refreshes the contents of the screen
 * 
 * @param x 0 if it's in single payer, 1 for for player using chromosome X, 2 for player using chromosome Y
 */
void arrowProcessing(int x);

/**
 * @brief Sets the chromosome the user chose and that will be used in that game
 */
void set_current_player_cromossoma(int cr);

int game(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);
