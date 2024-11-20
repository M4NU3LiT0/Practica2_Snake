#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>

#define W LED_MATRIX_0_WIDTH
#define H LED_MATRIX_0_HEIGHT

void set_pixel(unsigned int x, unsigned int y, unsigned int color);
void delay();
void spawn_snake();
void spawn_apple();
void clear();
void bordes();
void movimiento(unsigned int mov);
int check_apple_collision();
void delete_apple();
void grow_snake();

int vertical_orientation = 0;  // 0 = horizontal, 1 = vertical

volatile unsigned int * d_pad_u = D_PAD_0_UP;
volatile unsigned int * d_pad_d = D_PAD_0_DOWN;
volatile unsigned int * d_pad_l = D_PAD_0_LEFT;
volatile unsigned int * d_pad_r = D_PAD_0_RIGHT;

//led max (ajustados para los bordes)
unsigned short max_x = 31; // Reducido para los bordes
unsigned short max_y = 21; // Reducido para los bordes

//apple coords
unsigned int ax = 0;
unsigned int ay = 0;

// Snake coordinates arrays (dos columnas para el ancho)
#define MAX_SNAKE_LENGTH 20
#define INITIAL_LENGTH 4
unsigned int current_length = INITIAL_LENGTH;
unsigned int snake_x[MAX_SNAKE_LENGTH][2];
unsigned int snake_y[MAX_SNAKE_LENGTH][2];

// Punteros a la cabeza (2 pixeles)
unsigned int *head_x_left;
unsigned int *head_y_left;
unsigned int *head_x_right;
unsigned int *head_y_right;

// Punteros a la cola (2 pixeles)
unsigned int *tail_x_left;
unsigned int *tail_y_left;
unsigned int *tail_x_right;
unsigned int *tail_y_right;

//colors
unsigned int snake_color = 0x00FF0000;  // Rojo
unsigned int apple_color = 0x0000FF00;  // Verde
unsigned int border_color = 0x000000FF; // Azul

void main() {
    clear();
    bordes();
    
    // Inicializar la serpiente en posición horizontal
    for(int i = 0; i < current_length; i++) {
        // Columna izquierda
        snake_x[i][0] = 17 + i;
        snake_y[i][0] = 12;
        // Columna derecha (arriba cuando es horizontal)
        snake_x[i][1] = 17 + i;
        snake_y[i][1] = 13;
    }
    
    vertical_orientation = 0;
    
    // Inicializar punteros
    head_x_left = &snake_x[current_length-1][0];
    head_y_left = &snake_y[current_length-1][0];
    head_x_right = &snake_x[current_length-1][1];
    head_y_right = &snake_y[current_length-1][1];
    
    tail_x_left = &snake_x[0][0];
    tail_y_left = &snake_y[0][0];
    tail_x_right = &snake_x[0][1];
    tail_y_right = &snake_y[0][1];
    
    spawn_snake();

    unsigned int mov = 4;

    spawn_apple();
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
        
        if (check_apple_collision()) {
            delete_apple();
            grow_snake();
            spawn_apple();
        }
        
        delay();
    }
}

void bordes() {
    unsigned int *led_base = LED_MATRIX_0_BASE;
    for (int i = 0; i < W; i++) {
        *(led_base + i) = border_color; //arriba
        *(led_base + (W * (H - 1)) + i) = border_color; //abajo
    }
    for (int i = 0; i < H; i++) {
        *(led_base + (W * i)) = border_color; //izquierda
        *((led_base + (W * i)) + (W - 1)) = border_color; //derecha
    }
}

void clear() {
    unsigned int *led_base = LED_MATRIX_0_BASE;
    for (int i = 0; i < (W * H); i++) {
        *(led_base + i) = 0x00;
    }
}

int check_apple_collision() {
    if ((*head_x_left == ax && *head_y_left == ay) ||
        (*head_x_left == ax+1 && *head_y_left == ay) ||
        (*head_x_left == ax && *head_y_left == ay+1) ||
        (*head_x_left == ax+1 && *head_y_left == ay+1) ||
        (*head_x_right == ax && *head_y_right == ay) ||
        (*head_x_right == ax+1 && *head_y_right == ay) ||
        (*head_x_right == ax && *head_y_right == ay+1) ||
        (*head_x_right == ax+1 && *head_y_right == ay+1)) {
        return 1;
    }
    return 0;
}

void delete_apple() {
    set_pixel(ax, ay, 0);
    set_pixel(ax+1, ay, 0);
    set_pixel(ax, ay+1, 0);
    set_pixel(ax+1, ay+1, 0);
}

void grow_snake() {
    if (current_length >= MAX_SNAKE_LENGTH) return;
    
    for(int i = current_length; i > 0; i--) {
        snake_x[i][0] = snake_x[i-1][0];
        snake_y[i][0] = snake_y[i-1][0];
        snake_x[i][1] = snake_x[i-1][1];
        snake_y[i][1] = snake_y[i-1][1];
    }
    
    snake_x[0][0] = *tail_x_left;
    snake_y[0][0] = *tail_y_left;
    snake_x[0][1] = *tail_x_right;
    snake_y[0][1] = *tail_y_right;
    
    current_length++;
    
    head_x_left = &snake_x[current_length-1][0];
    head_y_left = &snake_y[current_length-1][0];
    head_x_right = &snake_x[current_length-1][1];
    head_y_right = &snake_y[current_length-1][1];
    
    tail_x_left = &snake_x[0][0];
    tail_y_left = &snake_y[0][0];
    tail_x_right = &snake_x[0][1];
    tail_y_right = &snake_y[0][1];
}

void movimiento(unsigned int mov) {
    unsigned int old_tail_x_left = *tail_x_left;
    unsigned int old_tail_y_left = *tail_y_left;
    unsigned int old_tail_x_right = *tail_x_right;
    unsigned int old_tail_y_right = *tail_y_right;
    
    for(int i = 0; i < current_length-1; i++) {
        snake_x[i][0] = snake_x[i+1][0];
        snake_y[i][0] = snake_y[i+1][0];
        snake_x[i][1] = snake_x[i+1][1];
        snake_y[i][1] = snake_y[i+1][1];
    }
    
    switch(mov) {
        case 1: //arriba
            if (!vertical_orientation) {
                vertical_orientation = 1;
                *head_x_right = *head_x_left + 1;
                *head_y_right = *head_y_left;
            }
            (*head_y_left)++;
            (*head_y_right)++;
            break;
            
        case 2: //abajo
            if (!vertical_orientation) {
                vertical_orientation = 1;
                *head_x_right = *head_x_left + 1;
                *head_y_right = *head_y_left;
            }
            (*head_y_left)--;
            (*head_y_right)--;
            break;
            
        case 3: //izquierda
            if (vertical_orientation) {
                vertical_orientation = 0;
                *head_x_right = *head_x_left;
                *head_y_right = *head_y_left + 1;
            }
            (*head_x_left)--;
            (*head_x_right)--;
            break;
            
        case 4: //derecha
            if (vertical_orientation) {
                vertical_orientation = 0;
                *head_x_right = *head_x_left;
                *head_y_right = *head_y_left + 1;
            }
            (*head_x_left)++;
            (*head_x_right)++;
            break;
    }
    
    set_pixel(old_tail_x_left, old_tail_y_left, 0);
    set_pixel(old_tail_x_right, old_tail_y_right, 0);
    
    set_pixel(*head_x_left, *head_y_left, snake_color);
    set_pixel(*head_x_right, *head_y_right, snake_color);
}

void spawn_snake() {
    for(int i = 0; i < current_length; i++) {
        set_pixel(snake_x[i][0], snake_y[i][0], snake_color);
        set_pixel(snake_x[i][1], snake_y[i][1], snake_color);
    }
}

void spawn_apple() {
    // Generar coordenadas entre 1 y max-1 para evitar los bordes
    ax = 1 + (rand() % (max_x - 1));
    ay = 1 + (rand() % (max_y - 1));

    printf("Manzana en: %d, %d\n", ax, ay);
    
    set_pixel(ax, ay, apple_color);
    set_pixel(ax+1, ay, apple_color);
    set_pixel(ax, ay+1, apple_color);
    set_pixel(ax+1, ay+1, apple_color);
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
