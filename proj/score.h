#pragma once

extern int score_to_print;

void score(int distance, int  *cdance);

void printScore(Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great);

void show_score(Bitmap * one, Bitmap * two, Bitmap * three, Bitmap * four, Bitmap * five, Bitmap * six, Bitmap * seven, Bitmap * eight, Bitmap * nine, Bitmap * zero);

void print_digit(int score, int x, Bitmap * one, Bitmap * two, Bitmap * three, Bitmap * four, Bitmap * five, Bitmap * six, Bitmap * seven, Bitmap * eight, Bitmap * nine, Bitmap * zero);

int number_of_digits(int number);

void reset_score_counter();
