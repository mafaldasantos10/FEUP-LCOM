#pragma once


int kbc_scan_ih();

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

uint8_t kbd_int_handler();

int kbd_scan_poll();

int kbd_poll_cmd();

void kbd_poll();
