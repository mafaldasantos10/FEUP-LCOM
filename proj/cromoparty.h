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

/**
 * @brief It estimates the number of pixels the powerup has to move in each frame
 * @param xi Initial position it can be either horizontal or vertical
 * @param xf Final position
 * @return
 */
int powerSpeed(int xi, int xf);

/**
 * @brief When the power up collides or reaches the end of the screen it changes its movements.
 * if it reaches the end of the screen the next movement will be in the opposite direction with a random y
 * if it collided the movement will start in x=0 and random y
 */
void changeDirect();

/**
 * @brief Changes the position where the power up will be drawn
 * @param xi Initial horizontal position
 * @param yi Initial vertical position
 * @param yf Final vertical position
 */
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

/**
 * @brief Interrupt mouse, timer and keyboard cycle of the game
 * @param bit_no_timer Timer identifier
 * @param bit_no_kb Keyboard identifier
 * @param bit_no_mouse Mouse identifier
 * @return
 */
int game(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse);
