#pragma once

extern bool click;

//STRUCTS
//////////////////////////////////////////////////////////////////

typedef struct {
  char name[25];
  int score;
  int rank;
  //int day;
  //int month;
  //int year;
} Player;

//PROTOTYPES
//////////////////////////////////////////////////////////////////

void score(int distance, int  *cdance);

void printScore();

void show_score(int score, int x, int y);

void print_digit(int number, int x, int y);

void print_small_digit(int number, int x, int y);

int number_of_digits(int number);

void reset_score_counter();

void reset_score_to_print();

void get_powerup();

void print_sentence(char string[], int x, int y);

int print_letter(char c, int x, int y);

void save_score();

void save_score_to_file();

void load_score_from_file();

int get_last_player_index();

int rank();

void print_high_scores();

void set_current_player_name(char name[25]);
