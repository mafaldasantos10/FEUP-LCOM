#pragma once

/** @defgroup Score Score
 * @{
 *
 * Functions to handle the Score of the player
 */

  extern bool click;

//STRUCTS
//////////////////////////////////////////////////////////////////

typedef struct {
  /** @brief Name of the player */
  char name[25];
  /** @brief Score of the player */
  int score;
  /** @brief Rank of the player */
  int rank;
  /** @brief Day the score was obtained */
  int day;
  /** @brief Month the score was obtained */
  int month;
  /** @brief Year the score was obtained */
  int year;
} Player;

//PROTOTYPES
//////////////////////////////////////////////////////////////////

/**
 * @brief Increases the score acordingly to the distance of the circle to the pad
 *
 * @param distance Distance in pixels between the circle and the pad
 * @param cdance Pointer to the variable that dictates the dance step the chromosome will do
 */
void score(int distance, int  *cdance);

/**
 * @brief Returns the player's current score
 *
 * @return int of the player's current score
 */
int getScore();

/**
 * @brief Prints one of the 5 possible prompts according to the player's performance on each move
 */
void printScore();

/**
 * @brief Prints a given score in a given position, from right to left. 
 * 
 * @param score The score to print. If "1" is received, it uses the player's current score 
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 */
void show_score(int score, int x, int y);

/**
 * @brief Prints the received number on the screen on the given coordinates
 * 
 * @param number Number to print
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 */
void print_digit(int number, int x, int y);

/**
 * @brief Prints the received number on the screen with a smaller set of numbers on the given coordinates
 * 
 * @param number Number to print
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 */
void print_small_digit(int number, int x, int y);

/**
 * @brief Finds the number of digits of an integer
 * 
 * @param number Number to use
 * @return Number of digits of the given number
 */
int number_of_digits(int number);

/**
 * @brief Resets the score counter global variable
 */
void reset_score_counter();

/**
 * @brief Resets the score_to_print global variable
 */
void reset_score_to_print();

/**
 * @brief Checks if the power up was clicked by the mouse
 */
void get_powerup();

/**
 * @brief Prints a sentence on the screen on the given coordinates
 * 
 * @param string[] Array of chars (string) with the sentence
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 */
void print_sentence(char string[], int x, int y);

/**
 * @brief Prints a letter (character) on the screen and adjusts the space between letters for a more natural looking writing
 * 
 * @param c Char to be printed
 * @param x Horizontal coordinate to print the score
 * @param y Vertical coordinate to print the score
 * 
 * @return Lenght the last printed letter
 */
int print_letter(char c, int x, int y);

/**
 * @brief If the players makes a highscore, it saves its information on the array of players, bearing in mind the array can be full.
 */
void save_score();

/**
 * @brief Saves the score in the highscores.txt file. Players are placed from the best to the worst
 */
void save_score_to_file();

/**
 * @brief Loads the information of the highscores.txt file
 */
void load_score_from_file();

/**
 * @brief Finds the index of the player with least points
 * 
 * @return Integer index of the last player
 */
int get_last_player_index();

/**
 * @brief Finds the correct rank of the player according to his score
 * 
 * @return Integer index of where he should be placed
 */
int rank();

/**
 * @brief Prints the score rankings in the high scores panel in order
 */
void print_high_scores();

/**
 * @brief Changes the player name
 * @param name New player name
 */
void set_current_player_name(char name[25]);

/**@}*/
