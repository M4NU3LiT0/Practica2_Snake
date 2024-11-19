#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>

void set_pixel(unsigned int x, unsigned int y, unsigned int color);
void delay();
void spawn_snake(unsigned int x, unsigned int y, unsigned int color);
void clear();
void movimiento(unsigned int mov);

volatile unsigned int * d_pad_u = D_PAD_0_UP;
volatile unsigned int * d_pad_d = D_PAD_0_DOWN;
volatile unsigned int * d_pad_l = D_PAD_0_LEFT;
volatile unsigned int * d_pad_r = D_PAD_0_RIGHT;

//led max
unsigned short max_x = 33;
unsigned short max_y = 23;

//snake coords
unsigned int x = 17;
unsigned int y = 12;

//snake dimensions
unsigned int snake_width = 2;
unsigned int snake_height = 4;

//colors
unsigned int snake_color = 0x0000FF00;  // Verde

void main() {
    clear();
    spawn_snake(x,y,snake_color);
   
    unsigned int mov = 4; // 1 = arriba, 2 = abajo, 3 = izq, 4 = derecha

    while (1) {
        if (*d_pad_u == 1 && mov != 2) {
            mov = 1;
        }
        if (*d_pad_d == 1 && mov != 1) {
            mov = 2;
        }
        if (*d_pad_l == 1 && mov != 4) {
            mov = 3;
        }
        if (*d_pad_r == 1 && mov != 3) {
            mov = 4;
        }
        movimiento(mov);
        delay();
    }
}

void movimiento(unsigned int mov) {
    switch(mov) {
        case 1://arriba
            spawn_snake(x,y,0);
            y += 1;
            spawn_snake(x,y,snake_color);
            break;
        case 2://abajo
            spawn_snake(x,y,0);
            y -= 1;
            spawn_snake(x,y,snake_color);
            break;
        case 3://izq
            spawn_snake(x,y,0);
            x -= 1;
            spawn_snake(x,y,snake_color);
            break;
        case 4://derecha
            spawn_snake(x,y,0);
            x += 1;
            spawn_snake(x,y,snake_color);
            break;
    }
}

//test para cambiar el cuadrado
void spawn_snake(unsigned int x, unsigned int y, unsigned int color) {
    for(int i = 0; i < snake_width; i++) {
        for(int j = 0; j < snake_height; j++) {
            set_pixel(x + i, y + j, color);
        }
    }
}

void set_pixel(unsigned int x, unsigned int y, unsigned int color) {
    unsigned int *led_base = LED_MATRIX_0_BASE;
    unsigned int *address;
    unsigned int offset;

    offset = x + (24-y) * LED_MATRIX_0_WIDTH;
    address = led_base + offset;
    *(address) = color;
}

void delay() {
    for (volatile int i = 0; i < 2000; i++) {}
}

void clear() {
    unsigned int *led_base = LED_MATRIX_0_BASE;
    for (int i = 0; i < LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT; i++) {
        *(led_base + i) = 0x00;
    }
}
