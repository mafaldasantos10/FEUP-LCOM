#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <minix/sysutil.h> 

#include "i8042.h"
#include "i8254.h"
#include "cromoparty.h"
#include "keyboard.h"
#include "interface.h"
#include "PS2mouse.h"
#include "menu.h"
#include "score.h"
#include "rtc.h"
#include "serialPort.h"

//VARIABLE INITIALIZATION
Arrow **arrows;
size_t number_of_arrows = 2; // number or arrows at the same time on the screen
int cromossoma_choice; // 0 - CrY | 1 - CrX
int cromossomaDance = 4; // defaul dance = idle
int colision = 6;
bool powerup = true;
int powerx = 0;
int powery = 0;
int xi = 0;
int yi = 0;
int yf = 300;


//FUNCTIONS
//////////////////////////////////////////////////////////////////

void init_arrows()
{
    arrows = (Arrow**)malloc(2 * sizeof(Arrow*));

    for (unsigned int i = 0; i < number_of_arrows; i++)
    {
        arrows[i] = (Arrow*)malloc(sizeof (Arrow));
        arrows[i]->currentX = 0;
        arrows[i]->active = true;
        arrows[i]->speed = 0;
        arrows[i]->direction = 0;
    }

    arrows[1]->direction = 3;
    arrows[0]->speed = 3;
    arrows[1]->speed = 8;
}

//////////////////////////////////////////////////////////////////

void print_queue()
{
    drawBitmap(images.background, 0, 0, ALIGN_LEFT);
    drawBitmap(images.pad, 462, 450, ALIGN_LEFT);
    changeDirect();
    drawBitmap(images.pointer, get_mouseX(), get_mouseY(), ALIGN_LEFT);
    show_score(1, get_horizontal_resolution(), 90);
    print_time(984, 17);
}

//////////////////////////////////////////////////////////////////

int pix_map_move_pos(int x)
{
    if (timer_counter % (sys_hz() / FRAME_RATE) == 0)
    {
        print_queue();

        if(x == 1)
        {
            drawBitmap(images.scoreY, 844, 180, ALIGN_LEFT);
            show_score((int)getContainery(), get_horizontal_resolution(), 217); 
        }
        else if (x == 2)
        {
            drawBitmap(images.scoreX, 844, 180, ALIGN_LEFT);
            show_score((int)getContainerx(), get_horizontal_resolution(), 217); 
        }

        for (unsigned int i = 0; i < number_of_arrows; i++)
        {
            if (arrows[i]->currentX >= get_horizontal_resolution())
            {
                cromossomaDance = 4;
                score(200, &cromossomaDance); //case that always evaluates to fail
                arrows[i]->active = false;
            }
            else
            {
                arrows[i]->currentX += arrows[i]->speed;
            }

            switch (arrows[i]->direction)
            {
                case 0:
                {
                    drawBitmap(images.arrow_up, arrows[i]->currentX, 450, ALIGN_LEFT);
                    break;
                }
                case 1:
                {
                    drawBitmap(images.arrow_left, arrows[i]->currentX, 450, ALIGN_LEFT);
                    break;
                }
                case 2:
                {
                    drawBitmap(images.arrow_down, arrows[i]->currentX, 450, ALIGN_LEFT);
                    break;
                }
                case 3:
                {
                    drawBitmap(images.arrow_right, arrows[i]->currentX, 450, ALIGN_LEFT);
                    break;
                }
            }
        }
    }
    
    printScore();
    printDance();
    double_buffer_to_video_mem();

    return 0;
}

//////////////////////////////////////////////////////////////////

int arrowRate(int i) 
{
    arrows[i]->speed = 2 + rand() % 4;  
    arrows[i]->direction = rand() % 4;

    return 0;
}

//////////////////////////////////////////////////////////////////

void keyboardArrows()
{
    int i;

    if (abs(arrows[0]->currentX - 462) < abs(arrows[1]->currentX - 462))
    {
        i = 0;
    }
    else
    {
        i = 1;
    }

    if (status == W_KEY && arrows[i]->direction == 0)
    {
        arrows[i]->active = false;
        cromossomaDance = 0;
        score(arrows[i]->currentX, &cromossomaDance);
        return;
    }
    else if (status == A_KEY && arrows[i]->direction == 1)
    {
        arrows[i]->active = false;
        cromossomaDance = 1;
        score(arrows[i]->currentX, &cromossomaDance);
        return;
    }
    else if (status == S_KEY && arrows[i]->direction == 2)
    {
        arrows[i]->active = false;
        cromossomaDance = 2;
        score(arrows[i]->currentX, &cromossomaDance);
        return;
    } 
    else if (status == D_KEY && arrows[i]->direction == 3)
    {
        arrows[i]->active = false;
        cromossomaDance = 3;
        score(arrows[i]->currentX, &cromossomaDance);
        return;
    }
    else
    {
        cromossomaDance = 4;
        score(200, &cromossomaDance); //case that evaluates to fail
        arrows[i]->active = false;
    }
}

//////////////////////////////////////////////////////////////////

void printDance()
{
    switch (cromossomaDance)
    {
        case 0:
        {
            /* Y */
            if (!cromossoma_choice)
                drawBitmap(images.cromossomaY_up, 650, 300, ALIGN_LEFT);
            /* X */
            else
                drawBitmap(images.cromossomaX_up, 650, 300, ALIGN_LEFT);
            break;
        }
        case 1:
        {
            /* Y */
            if (!cromossoma_choice)
                drawBitmap(images.cromossomaY_right, 650, 300, ALIGN_LEFT);
            /* X */
            else
                drawBitmap(images.cromossomaX_right, 650, 300, ALIGN_LEFT);
            break;
        }
        case 2:
        {
            /* Y */
            if (!cromossoma_choice)
                drawBitmap(images.cromossomaY_down, 650, 300, ALIGN_LEFT);
            /* X */
            else
                drawBitmap(images.cromossomaX_down, 650, 300, ALIGN_LEFT);
            break;
        }
        case 3:
        {
            /* Y */
            if (!cromossoma_choice)
                drawBitmap(images.cromossomaY_left, 650, 300, ALIGN_LEFT);
            /* X */
            else
                drawBitmap(images.cromossomaX_left, 650, 300, ALIGN_LEFT);
            break;
        }
        case 4:
        {
            /* Y */
            if (!cromossoma_choice)
                drawBitmap(images.cromossomaY_idle, 650, 300, ALIGN_LEFT);
            /* X */
            else
                drawBitmap(images.cromossomaX_idle, 650, 300, ALIGN_LEFT);
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////

int powerSpeed(int xi, int xf)
{
    int speedx = abs(xf - xi) / 120;

    return speedx;
}

void changeDirect()
{
    if (colision <= 4)
    {
        if (powerup)
        {
            powerUps(xi, yi, yf);
        }
        else
        {
            if (images.power->colided)
            {
                 if (powery < 368)
                {
                    yf = 468 +rand() % 250;
                }
                else
                {
                    yf = rand() % 250;
                }
                powerx = 0;
                images.power->colided = false;
            }
            else
            {
                yf = rand() % 691;
            }

            yi = powery;
            xi = powerx;
            powerup = true;

        }
    }
    else
    {
        if ((timer_counter % 1200) == 0)
        {
            colision = 0;
        }
    }
}

void powerUps(int xi, int yi, int yf)
{
    int xf;
    if (xi == 0)
    {
        xf = 954;       
    }
    else
    {
        xf = 0;
    }

    int speedx = powerSpeed(xi, xf);
    int speedy = powerSpeed(yi, yf);

    if (click)
    {
        powerup = false;
        colision = 5;
        click = false;
        images.power->colided = true;
        return;
    }

    if (!cromossoma_choice)
    {
        if (images.cromossomaY_up->colided || images.cromossomaY_right->colided || images.cromossomaY_left->colided || images.cromossomaY_down->colided || images.arrow_up->colided || images.arrow_right->colided 
        || images.arrow_left->colided || images.arrow_down->colided || images.cromossomaY_idle->colided)
        {          
            images.cromossomaY_up->colided = false;
            images.cromossomaY_right->colided = false;
            images.cromossomaY_left->colided = false;
            images.cromossomaY_down->colided = false;
            images.cromossomaY_idle->colided = false;
            images.arrow_up->colided = false;
            images.arrow_right->colided = false;
            images.arrow_left->colided = false;
            images.arrow_down->colided = false;
            images.power->colided = true;
            powerup = false;
            colision++;
            return;
        }
    }
    else
    {
        if (images.cromossomaX_up->colided || images.cromossomaX_right->colided || images.cromossomaX_left->colided || images.cromossomaX_down->colided || images.arrow_up->colided || images.arrow_right->colided 
        || images.arrow_left->colided || images.arrow_down->colided || images.cromossomaX_idle->colided)
        {         
            images.cromossomaX_up->colided = false;
            images.cromossomaX_right->colided = false;
            images.cromossomaX_left->colided = false;
            images.cromossomaX_down->colided = false;
            images.cromossomaX_idle->colided = false;
            images.arrow_up->colided = false;
            images.arrow_right->colided = false;
            images.arrow_left->colided = false;
            images.arrow_down->colided = false;
            images.power->colided = true;
            powerup = false;
            colision++;
            return;
        }
    }

    if (xi >= xf)
    {
         if ((powerx - speedx) <= xf)
        {
            powerx = xf;    
            powerup = false; 
            colision++;
        }
        else
        {
            powerx -= speedx;
        }
    }

    if (xi < xf)
    {
         if ((powerx + speedx) >= xf)
        {
            powerx = xf;    
            powerup = false;
            colision++; 
        }
        else
        {
            powerx += speedx;
        }
    }

    if (yi >= yf)
    {
        if (powery - speedy <= yf) 
        {
            powery = yf;
        }
        else
        {
            powery -= speedy;
        }
    }

    if (yi < yf)
    {
        if (powery + speedy >= yf) 
        {
            powery = yf;
        }
        else
        {
            powery += speedy;
        }
    }

    drawBitmap(images.power, powerx, powery, ALIGN_LEFT);
}

void reset_powerup()
{
    xi = 0;
    yf = 0;
    yi = 0;
    powerx = 0;
    powery = 0;
    colision = 6;
}

//////////////////////////////////////////////////////////////////

void reset_game()
{
    timer_counter = 0;
    cromossomaDance = 4;
    reset_score_to_print();
    reset_score_counter();
    reset_powerup();
}

//////////////////////////////////////////////////////////////////

void end_game()
{
    save_score();
    drawBitmap(images.end, 0, 0, ALIGN_LEFT);
    show_score(1, 630, 615); /* 1 - current score */
    double_buffer_to_video_mem();
    sleep(3);
    default_state();
    reset_game();
}

//////////////////////////////////////////////////////////////////

void arrowProcessing(int x)
{
    if (!arrows[0]->active)
    {
        arrowRate(0);
        arrows[0]->currentX = 0;
        arrows[0]->active = true;
        return;
    }
    else if (!arrows[1]->active)
    {
        arrowRate(1);
        arrows[1]->currentX = 0;
        arrows[1]->active = true;
        return;
    }
    else
    {
        pix_map_move_pos(x);
    }
}

//////////////////////////////////////////////////////////////////

int game(uint8_t bit_no_timer, uint8_t bit_no_kb, uint8_t bit_no_mouse)
{
    init_arrows();

    drawBitmap(images.background, 0, 0, ALIGN_LEFT);
    if (!cromossoma_choice)
    {
        drawBitmap(images.cromossomaY_idle, 650, 300, ALIGN_LEFT);
    }
    else
    {
        drawBitmap(images.cromossomaX_idle, 650, 300, ALIGN_LEFT);
    }
    drawBitmap(images.pad, 462, 450, ALIGN_LEFT);
    double_buffer_to_video_mem();

    uint32_t irq_set_timer = BIT(bit_no_timer);
    uint32_t irq_set_keyboard = BIT(bit_no_kb);
    uint32_t irq_set_mouse = BIT(bit_no_mouse);
    
    uint8_t byte1[1], byte2[2];
    int ipc_status, r, size = 1, s = 1;
    bool esc = false, p_key = false, wait = false;
    message msg;

    while (!esc)
    {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status))
        { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & irq_set_timer)
                    {
                        if (!p_key)
                        {
                            timer_int_handler();
                            arrowProcessing(0);
                        }
                    }

                    if (msg.m_notify.interrupts & irq_set_keyboard)
                    { /* subscribed interrupt */
                    
                        kbc_asm_ih();

                        if (status == MSB)
                        {
                            wait = true;
                            continue;
                        }
                        if (wait == true)
                        {
                            wait = false;
                            size = 2;
                        }

                        if (status == ESC_BK)
                        {
                            esc = true;
                        }

                        if (status == P_KEY_BK)
                        {
                            if (!p_key)
                            {
                                p_key = true;
                                drawBitmap(images.pause, 180, 116, ALIGN_LEFT);
                                double_buffer_to_video_mem();
                            }
                            else
                            {
                                p_key = false;
                            }
                        }

                        if (status == W_KEY || status == A_KEY || status == S_KEY ||status  == D_KEY)
                        {
                            keyboardArrows();
                        }

                        if (size == 1)
                        {
                            byte1[0] = status;
                        }

                        if (size == 2)
                        {
                            byte2[0] = MSB;
                            byte2[1] = status;
                        }
                    }

                    if (msg.m_notify.interrupts & irq_set_mouse)
                    { /* subscribed interrupt */

                        mouse_ih();

                        if (s == 1)
                        {
                            if (status_mouse & BIT(3))
                            {
                                pp.bytes[0] = status_mouse;
                                s++;
                            }
                            continue;
                        } 

                        if (s == 2)
                        {
                            pp.bytes[1] = status_mouse;
                            s++;
                            continue;
                        }
                        
                        if (s == 3)
                        {
                            pp.bytes[2] = status_mouse;
                            packet_create();
                            currentMousePosition();
                            get_powerup();
                            s = 1;
                        }
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        }
        else
        {   /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }

        /* game ends after 45 seconds */
        if (timer_counter > 2700)
        {   
            end_game();
            return 0;
        }

        size = 1;
    }

    /* to reset global variables for a new game */
    if (esc)
    {   
        end_game();
    }
    
    return 0;
}

///////////////////////////////////////////////////////////////////

void set_current_player_cromossoma(int cr)
{
  cromossoma_choice = cr;
}

