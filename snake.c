#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>

void set_pixel(unsigned int x, unsigned int y, unsigned int color);
void delay();
void spawn_snake();
void spawn_apple();
void clear();
void movimiento(unsigned int mov);

int vertical_orientation = 0;  // 0 = horizontal, 1 = vertical

volatile unsigned int * d_pad_u = D_PAD_0_UP;
volatile unsigned int * d_pad_d = D_PAD_0_DOWN;
volatile unsigned int * d_pad_l = D_PAD_0_LEFT;
volatile unsigned int * d_pad_r = D_PAD_0_RIGHT;

//led max - reducidos en 1 para asegurar que la manzana 2x2 quepa
unsigned short max_x = 32;
unsigned short max_y = 22;

//apple coords
unsigned int ax = 0;
unsigned int ay = 0;

// Snake coordinates arrays (dos columnas para el ancho)
#define SNAKE_LENGTH 4
unsigned int snake_x[SNAKE_LENGTH][2];  // [segmento][columna]
unsigned int snake_y[SNAKE_LENGTH][2];  // [segmento][columna]

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
unsigned int snake_color = 0x0000FF00;  // Verde
unsigned int apple_color = 0x00FF00FF;  // Rosa

void main() {
    clear();
    
    // Inicializar la serpiente en posición horizontal
    for(int i = 0; i < SNAKE_LENGTH; i++) {
        // Columna izquierda
        snake_x[i][0] = 17 + i;
        snake_y[i][0] = 12;
        // Columna derecha (arriba cuando es horizontal)
        snake_x[i][1] = 17 + i;
        snake_y[i][1] = 13;  // Un pixel arriba para el ancho
    }
    
    vertical_orientation = 0;  // Comenzar en orientación horizontal
    
    // Inicializar punteros
    // Cabeza (último segmento)
    head_x_left = &snake_x[SNAKE_LENGTH-1][0];
    head_y_left = &snake_y[SNAKE_LENGTH-1][0];
    head_x_right = &snake_x[SNAKE_LENGTH-1][1];
    head_y_right = &snake_y[SNAKE_LENGTH-1][1];
    
    // Cola (primer segmento)
    tail_x_left = &snake_x[0][0];
    tail_y_left = &snake_y[0][0];
    tail_x_right = &snake_x[0][1];
    tail_y_right = &snake_y[0][1];
    
    // Dibujar serpiente inicial
    spawn_snake();

    unsigned int mov = 4; // 1 = arriba, 2 = abajo, 3 = izq, 4 = derecha

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
        delay();
    }
}

void movimiento(unsigned int mov) {
    // Guardar la posición antigua de la cola
    unsigned int old_tail_x_left = *tail_x_left;
    unsigned int old_tail_y_left = *tail_y_left;
    unsigned int old_tail_x_right = *tail_x_right;
    unsigned int old_tail_y_right = *tail_y_right;
    
    // Mover cada segmento a la posición del siguiente
    for(int i = 0; i < SNAKE_LENGTH-1; i++) {
        snake_x[i][0] = snake_x[i+1][0];
        snake_y[i][0] = snake_y[i+1][0];
        snake_x[i][1] = snake_x[i+1][1];
        snake_y[i][1] = snake_y[i+1][1];
    }
    
    // Mover la cabeza según la dirección
    switch(mov) {
        case 1: //arriba
            if (!vertical_orientation) {
                // Cambiar de horizontal a vertical
                vertical_orientation = 1;
                // Ajustar el segundo pixel para estar a la derecha en vez de arriba
                *head_x_right = *head_x_left + 1;
                *head_y_right = *head_y_left;
            }
            (*head_y_left)++;
            (*head_y_right)++;
            break;
            
        case 2: //abajo
            if (!vertical_orientation) {
                // Cambiar de horizontal a vertical
                vertical_orientation = 1;
                // Ajustar el segundo pixel para estar a la derecha en vez de arriba
                *head_x_right = *head_x_left + 1;
                *head_y_right = *head_y_left;
            }
            (*head_y_left)--;
            (*head_y_right)--;
            break;
            
        case 3: //izquierda
            if (vertical_orientation) {
                // Cambiar de vertical a horizontal
                vertical_orientation = 0;
                // Ajustar el segundo pixel para estar arriba en vez de a la derecha
                *head_x_right = *head_x_left;
                *head_y_right = *head_y_left + 1;
            }
            (*head_x_left)--;
            (*head_x_right)--;
            break;
            
        case 4: //derecha
            if (vertical_orientation) {
                // Cambiar de vertical a horizontal
                vertical_orientation = 0;
                // Ajustar el segundo pixel para estar arriba en vez de a la derecha
                *head_x_right = *head_x_left;
                *head_y_right = *head_y_left + 1;
            }
            (*head_x_left)++;
            (*head_x_right)++;
            break;
    }
    
    // Borrar la cola antigua
    set_pixel(old_tail_x_left, old_tail_y_left, 0);
    set_pixel(old_tail_x_right, old_tail_y_right, 0);
    
    // Dibujar la nueva cabeza
    set_pixel(*head_x_left, *head_y_left, snake_color);
    set_pixel(*head_x_right, *head_y_right, snake_color);
}

void spawn_snake() {
    // Dibujar cada segmento de la serpiente
    for(int i = 0; i < SNAKE_LENGTH; i++) {
        set_pixel(snake_x[i][0], snake_y[i][0], snake_color);  // Columna izquierda
        set_pixel(snake_x[i][1], snake_y[i][1], snake_color);  // Columna derecha
    }
}

void spawn_apple() {
    // Generar coordenadas aleatorias asegurando que quepa la manzana 2x2
    ax = rand() % (max_x);
    ay = rand() % (max_y);

    printf("Manzana en: %d, %d\n", ax, ay);
    
    // Dibujar manzana 2x2
    set_pixel(ax, ay, apple_color);       // Superior izquierda
    set_pixel(ax+1, ay, apple_color);     // Superior derecha
    set_pixel(ax, ay+1, apple_color);     // Inferior izquierda
    set_pixel(ax+1, ay+1, apple_color);   // Inferior derecha
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
