#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>

void set_pixel(unsigned int x, unsigned int y, unsigned int color);
void delay();
void spawn_snake();
void clear();
void movimiento(unsigned int mov);

volatile unsigned int * d_pad_u = D_PAD_0_UP;
volatile unsigned int * d_pad_d = D_PAD_0_DOWN;
volatile unsigned int * d_pad_l = D_PAD_0_LEFT;
volatile unsigned int * d_pad_r = D_PAD_0_RIGHT;

//led max
unsigned short max_x = 33;
unsigned short max_y = 23;


// para guardar el historial de posiciones}
#define SNAKE_LENGTH 4
unsigned int snake_x[SNAKE_LENGTH];  // coordenadas x/ segmento
unsigned int snake_y[SNAKE_LENGTH];  // coordenadas y / segmento

// Punteros a la cabeza y cola
unsigned int *head_x;
unsigned int *head_y;
unsigned int *tail_x;
unsigned int *tail_y;

//colors
unsigned int snake_color = 0x0000FF00;  // Verde

void main() {
    clear();
    
    // Inicializar la serpiente en posición horizontal
    for(int i = 0; i < SNAKE_LENGTH; i++) {
        snake_x[i] = 17 + i;  // Comienza en x=17
        snake_y[i] = 12;      // Misma y para todos
    }
    
    // Inicializar pointers
    head_x = &snake_x[SNAKE_LENGTH-1];  // Último elemento (más a la derecha)
    head_y = &snake_y[SNAKE_LENGTH-1];
    tail_x = &snake_x[0];                // Primer elemento (más a la izquierda)
    tail_y = &snake_y[0];
    
    // Dibujar serpiente inicial
    spawn_snake();

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
    // Guardar la posición antigua de la cola
    unsigned int old_tail_x = *tail_x;
    unsigned int old_tail_y = *tail_y;
    
    // Mover cada segmento a la posición del siguiente
    for(int i = 0; i < SNAKE_LENGTH-1; i++) {
        snake_x[i] = snake_x[i+1];
        snake_y[i] = snake_y[i+1];
    }
    
    // Mover la cabeza según la dirección
    switch(mov) {
        case 1://arriba
            (*head_y)++;
            break;
        case 2://abajo
            (*head_y)--;
            break;
        case 3://izq
            (*head_x)--;
            break;
        case 4://derecha
            (*head_x)++;
            break;
    }
    
    // Borrar la cola antigua
    set_pixel(old_tail_x, old_tail_y, 0);
    
    // Dibujar la nueva cabeza
    set_pixel(*head_x, *head_y, snake_color);
}

void spawn_snake() {
    // Dibujar cada segmento de la serpiente
    for(int i = 0; i < SNAKE_LENGTH; i++) {
        set_pixel(snake_x[i], snake_y[i], snake_color);
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
