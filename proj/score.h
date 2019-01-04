#pragma once

extern bool click;

//STRUCTS
//////////////////////////////////////////////////////////////////

typedef struct {
  char name[25];
  int score;
  int rank;
  int day;
  int month;
  int year;
} Player;

//PROTOTYPES
//////////////////////////////////////////////////////////////////

/**
 * @brief Increases the score acordingly to the distance of the circle to the pad
 *
 * @param distance Distance in pixels between the circle and the pad
 * @param cdance ?????????????????????????????????????????????????????????
 */
void score(int distance, int  *cdance);

int getScore();

/**
 * @brief Acordingly to the distance it prints a different message in the screen
 */
void printScore();

/**
 * @brief Sends the right integer to be printed by the print_digit function
 * @param score The score to print
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 */
void show_score(int score, int x, int y);

/**
 * @brief Prints the recieved number on the screen
 * @param number Number to print
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 */
void print_digit(int number, int x, int y);

/**
 * @brief Prints the recieved number on the screen with the smaller set of numbers 
 * @param number Number to print
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 */
void print_small_digit(int number, int x, int y);

/**
 * @brief Number of digits an integer has
 * @param number Number to print
 * @return Number of digits of the parameter number
 */
int number_of_digits(int number);

/**
 * @brief resets the score counter global variable
 */
void reset_score_counter();

/**
 * @brief resets the score_to_print global variable
 */
void reset_score_to_print();

/**
 * @brief Checks if the mouse clicked on the power up
 */
void get_powerup();

/**
 * @brief Prints a sentence on the screen
 * @param string[] array of chars with the sentence
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 */
void print_sentence(char string[], int x, int y);

/**
 * @brief Prints a letter on the screen and adjustes the space between letters 
 * @param c char to be printed
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 * @return Lengh of which letter bitmap
 */
int print_letter(char c, int x, int y);

/**
 * @brief ??????????????????????////
 */
void save_score();

/**
 * @brief Saves the score in the highscore file
 */
void save_score_to_file();

/**
 * @brief Loads the information of the file
 */
void load_score_from_file();

int get_last_player_index();

int rank();

/**
 * @brief Prints the score rankings in the high score panel
 */
void print_high_scores();

/**
 * @brief Changes the player name
 * @param name[25] New player name
 */
void set_current_player_name(char name[25]);
