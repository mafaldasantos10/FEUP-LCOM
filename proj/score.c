#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"
#include "cromoparty.h"
#include "score.h"

//VARIABLE INITIALIZATION
int score_to_print = 0;
int score_counter = 0;

//FUNCTIONS
//////////////////////////////////////////////////////////////////

void score(int distance, int *cdance)
{
  if (abs(462 - distance) < 10)
  {
    score_counter += 30;
    score_to_print = 1;
    return;
  }
  else if (abs(462 - distance) < 35)
  {
    score_counter += 10;
    score_to_print = 2;
    return;
  }
  else if (abs(462 - distance) < 70)
  {
    score_counter += 5;
    score_to_print = 3;
    return;
  }
  else
  {
    score_to_print = 4;
    if (!(score_counter - 5 < 0))
    {
      score_counter -= 5;
    }
    else
    {
      score_counter = 0;
    }
    *cdance = 4;
  }
}

//////////////////////////////////////////////////////////////////

void printScore(Bitmap * okay, Bitmap * miss, Bitmap * perfect, Bitmap * great)
{ 
  switch (score_to_print)
  {
    case 0:
      //prints nothing
      break;
    case 1:
    {
      drawBitmap(perfect, 362, 330, ALIGN_LEFT);
      break;
    }
    case 2:
    {
      drawBitmap(great, 362, 330, ALIGN_LEFT);
      break;
    }
    case 3:
    {
      drawBitmap(okay, 362, 330, ALIGN_LEFT);
      break;
    }
    case 4:
    {
      drawBitmap(miss, 362, 330, ALIGN_LEFT);
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////

void show_score(Bitmap * one, Bitmap * two, Bitmap * three, Bitmap * four, Bitmap * five, Bitmap * six, Bitmap * seven, Bitmap * eight, Bitmap * nine, Bitmap * zero)
{
  int digits = number_of_digits(score_counter);
  int gap = 150; //distance between numbers
  int score = score_counter;

  for (int i = 0; i < digits; i++)
  {
    print_digit(score % 10, 1024 - gap, one, two, three, four, five, six, seven, eight, nine, zero);
    score /= 10;
    gap += 100;
  }
}

//////////////////////////////////////////////////////////////////

void print_digit(int score, int x, Bitmap * one, Bitmap * two, Bitmap * three, Bitmap * four, Bitmap * five, Bitmap * six, Bitmap * seven, Bitmap * eight, Bitmap * nine, Bitmap * zero)
{
  switch (score)
  {
    case 0:
    {
      drawBitmap(zero, x, 100, ALIGN_LEFT);
      break;
    }
    case 1:
    {
      drawBitmap(one, x, 100, ALIGN_LEFT);
      break;
    }
    case 2:
    {
      drawBitmap(two, x, 100, ALIGN_LEFT);
      break;
    }
    case 3:
    {
      drawBitmap(three, x, 100, ALIGN_LEFT);
      break;
    }
    case 4:
    {
      drawBitmap(four, x, 100, ALIGN_LEFT);
      break;
    }
    case 5:
    {
      drawBitmap(five, x, 100, ALIGN_LEFT);
      break;
    }
    case 6:
    {
      drawBitmap(six, x, 100, ALIGN_LEFT);
      break;
    }
    case 7:
    {
      drawBitmap(seven, x, 100, ALIGN_LEFT);
      break;
    }
    case 8:
    {
      drawBitmap(eight, x, 100, ALIGN_LEFT);
      break;
    }
    case 9:
    {
      drawBitmap(nine, x, 100, ALIGN_LEFT);
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////

int number_of_digits(int number)
{
  int digits = 0;
  do{ 
    number /= 10; digits++; 
  } while (number != 0);

  return digits;
}
