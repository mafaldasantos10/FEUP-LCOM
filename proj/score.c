#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"
#include "cromoparty.h"
#include "interface.h"
#include "PS2mouse.h"
#include "score.h"

//VARIABLE INITIALIZATION
int score_to_print = 0;
int score_counter = 0;
bool click = false;

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

void printScore()
{ 
  switch (score_to_print)
  {
    case 0:
      //prints nothing
      break;
    case 1:
    {
      drawBitmap(images.perfect, 362, 330, ALIGN_LEFT);
      break;
    }
    case 2:
    {
      drawBitmap(images.great, 362, 330, ALIGN_LEFT);
      break;
    }
    case 3:
    {
      drawBitmap(images.okay, 362, 330, ALIGN_LEFT);
      break;
    }
    case 4:
    {
      drawBitmap(images.miss, 362, 330, ALIGN_LEFT);
      break;
    }
     case 5:
    {
      drawBitmap(images.extra, 362, 330, ALIGN_LEFT);
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////

void show_score(int x, int y)
{
  int digits = number_of_digits(score_counter);
  int gap = 75; //distance between numbers
  int score = score_counter;

  for (int i = 0; i < digits; i++)
  {
    print_digit(score % 10, x - gap, y);
    score /= 10;
    gap += 60; // gap increases with each extra digit
  }
}

//////////////////////////////////////////////////////////////////

void print_digit(int score, int x, int y)
{
  switch (score)
  {
    case 0:
    {
      drawBitmap(images.zero, x, y, ALIGN_LEFT);
      break;
    }
    case 1:
    {
      drawBitmap(images.one, x, y, ALIGN_LEFT);
      break;
    }
    case 2:
    {
      drawBitmap(images.two, x, y, ALIGN_LEFT);
      break;
    }
    case 3:
    {
      drawBitmap(images.three, x, y, ALIGN_LEFT);
      break;
    }
    case 4:
    {
      drawBitmap(images.four, x, y, ALIGN_LEFT);
      break;
    }
    case 5:
    {
      drawBitmap(images.five, x, y, ALIGN_LEFT);
      break;
    }
    case 6:
    {
      drawBitmap(images.six, x, y, ALIGN_LEFT);
      break;
    }
    case 7:
    {
      drawBitmap(images.seven, x, y, ALIGN_LEFT);
      break;
    }
    case 8:
    {
      drawBitmap(images.eight, x, y, ALIGN_LEFT);
      break;
    }
    case 9:
    {
      drawBitmap(images.nine, x, y, ALIGN_LEFT);
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

//////////////////////////////////////////////////////////////////

void reset_score_counter()
{
  score_counter = 0;
}

//////////////////////////////////////////////////////////////////

void reset_score_to_print()
{
  score_to_print = 0;
}

//////////////////////////////////////////////////////////////////

void get_powerup()
{
  if (pp.lb && images.pointer->colided)
	{
		score_counter += 100;
    click = true;
    score_to_print = 5;
	}

  images.pointer->colided = false;
}
