#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>

#define W LED_MATRIX_0_WIDTH // width
#define H LED_MATRIX_0_HEIGHT // height

void set_pixel(unsigned int x, unsigned int y, unsigned int color); // setear pixel por posición y color
void delay();
void spawn_snake();
void spawn_apple();
void clear();
void bordes();
void movimiento(unsigned int mov); //manejar movimiento
int check_apple_collision();
void delete_apple();
void grow_snake();

int vertical_orientation = 0;  // 0 = horizontal y 1 = vertical
int score = 0;
int speed = 2000; //vamos a ir reduciendo esto si avanza la partid, debe estar a 100ms en ripes

volatile unsigned int * d_pad_u = D_PAD_0_UP;
volatile unsigned int * d_pad_d = D_PAD_0_DOWN;
volatile unsigned int * d_pad_l = D_PAD_0_LEFT;
volatile unsigned int * d_pad_r = D_PAD_0_RIGHT;

//ajustados  para generar las manzanas, sino no aparecen
unsigned short max_x = 31; //corregir para que funcione con el MAX_WIDTH
unsigned short max_y = 21; //apra el MAX_HEIGHT

//apple coords
unsigned int ax = 0;
unsigned int ay = 0;

// Snake coordinates arrays (dos columnas para el ancho)
#define MAX_SNAKE_LENGTH 20 // wea de test
#define INITIAL_LENGTH 4 // x lenght
unsigned int current_length = INITIAL_LENGTH; //para rastrear lenght  actual

/*
estructura principal que almacena las posiciones de la serpiente, array bidimensional
MAX_SNAKE_LENGTH cada segmento de la snake
[2] van a ser los dos píxeles de ancho que tiene la serpiente

example:
snake_x[0][0] y snake_y[0][0] son las coordenadas del píxel izquierdo del primer segmento
snake_x[0][1] y snake_y[0][1] son las coordenadas del píxel derecho del mismo segmento
*/
unsigned int snake_x[MAX_SNAKE_LENGTH][2]; 
unsigned int snake_y[MAX_SNAKE_LENGTH][2];

// pointers a la cabeza (2 pixeles), apuntan al último segmento del array (current_length - 1)
unsigned int *head_x_left;
unsigned int *head_y_left;
unsigned int *head_x_right;
unsigned int *head_y_right;

// Punteros a la cola, apuntan al primer segmento del array (índice 0)
unsigned int *tail_x_left;
unsigned int *tail_y_left;
unsigned int *tail_x_right;
unsigned int *tail_y_right;

//colors
unsigned int snake_color = 0x00FF0000;  // Rojo
unsigned int apple_color = 0x0000FF00;  // Verde
unsigned int border_color = 0x000000FF; // Azul

void main() {
    clear(); //limpiamos pixeles
    bordes(); //dibujar bordes
    
    // Inicializar la serpiente en horizontal, y al centro
    for(int i = 0; i < current_length; i++) { //for hasta 4
        // Columna izquierda
        snake_x[i][0] = 17 + i; //x=17, 18, 19...
        snake_y[i][0] = 12; //fijo y=12
        // Columna derecha (arriba cuando es horizontal)
        snake_x[i][1] = 17 + i;
        snake_y[i][1] = 13;
    }
    
    vertical_orientation = 0;
    
    // Inicializar punteros de la cabeza (uultimo egmento)
    head_x_left = &snake_x[current_length-1][0];
    head_y_left = &snake_y[current_length-1][0];
    head_x_right = &snake_x[current_length-1][1];
    head_y_right = &snake_y[current_length-1][1];
    
    //de la cola (primer segmento)
    tail_x_left = &snake_x[0][0];
    tail_y_left = &snake_y[0][0];
    tail_x_right = &snake_x[0][1];
    tail_y_right = &snake_y[0][1];
    
    spawn_snake(); //coloreamos snake

    unsigned int mov = 4; //a la derecha

    spawn_apple();
    while (1) { //bucle infinito del juego
        if (*d_pad_u == 1 && mov != 2) { //si dpad arriba y no va hacia abajo
            mov = 1;
        }
        if (*d_pad_d == 1 && mov != 1) { //si dpad abajo y no va hacia arriba
            mov = 2;
        }
        if (*d_pad_l == 1 && mov != 4) { //si dpad izquierda y no va a derecha
            mov = 3;
        }
        if (*d_pad_r == 1 && mov != 3) { //si dpad derera y no va a izquierda
            mov = 4;
        }
        movimiento(mov);//actualizar posicion
        
        if (check_apple_collision()) { //si se tocó una serpiente
            delete_apple(); //se borra
            grow_snake(); //Crece
            spawn_apple(); //otra manzana
        }
        
        delay();
    }
}

void bordes() {
    unsigned int *led_base = LED_MATRIX_0_BASE; //apuntador led por led
    for (int i = 0; i < W; i++) { //horizontales
        *(led_base + i) = border_color; //arriba
        *(led_base + (W * (H - 1)) + i) = border_color; //abajo
    }
    for (int i = 0; i < H; i++) { //verticales
        *(led_base + (W * i)) = border_color; //izquierda
        *((led_base + (W * i)) + (W - 1)) = border_color; //derecha
    }
}

void clear() {//pa limpiar pixeles
    unsigned int *led_base = LED_MATRIX_0_BASE;
    for (int i = 0; i < (W * H); i++) {
        *(led_base + i) = 0x00;
    }
}

int check_apple_collision() {
    /* aqui checo 8 posibles puntos de colisión porque:
        La cabeza de la serpiente tiene 2 píxeles y la manzana ocupa 4 píxeles (2x2)
        probablemente se puede optimizar mas pero solo se me ocurrió asi*/
    if (
        //pix izquierdo cabeza
        (*head_x_left == ax && *head_y_left == ay) ||
        (*head_x_left == ax+1 && *head_y_left == ay) ||
        (*head_x_left == ax && *head_y_left == ay+1) ||
        (*head_x_left == ax+1 && *head_y_left == ay+1) ||
        //pix derecho cabeza
        (*head_x_right == ax && *head_y_right == ay) ||
        (*head_x_right == ax+1 && *head_y_right == ay) ||
        (*head_x_right == ax && *head_y_right == ay+1) ||
        (*head_x_right == ax+1 && *head_y_right == ay+1)) {
        speed -=100; //reduzco el offset del delay pa que vaya mas rapido
        score++;

        return 1; //si colision
    }
    return 0; //no colision
}

void delete_apple() {  //seteamos la posicion de la manzana sin color
    set_pixel(ax, ay, 0);
    set_pixel(ax+1, ay, 0);
    set_pixel(ax, ay+1, 0);
    set_pixel(ax+1, ay+1, 0);
}

void grow_snake() {
    if (current_length >= MAX_SNAKE_LENGTH) return; //primero  checare si puede crecer mas
    /*es algo como: C=cola, N=Nuvo segmento, 1 y2= enmedio, H=head
        antes de crecer: [C][1][2][H]
        desps de crecer: [C][N][1][2][H]
    */
    //primero desplazamos todos los segmentos una posición hacia adelante
    for(int i = current_length; i > 0; i--) {
         // Copiamos las coordenadas del segmento anterior al actua
        snake_x[i][0] = snake_x[i-1][0];
        snake_y[i][0] = snake_y[i-1][0];
        snake_x[i][1] = snake_x[i-1][1];
        snake_y[i][1] = snake_y[i-1][1];
    }
    //copiams la posición actual de la cola al nuevo primer segmento (nueva cola)
    snake_x[0][0] = *tail_x_left;
    snake_y[0][0] = *tail_y_left;
    snake_x[0][1] = *tail_x_right;
    snake_y[0][1] = *tail_y_right;
    
    current_length++;
    
    //recorrer pointers de la cabeza
    head_x_left = &snake_x[current_length-1][0];
    head_y_left = &snake_y[current_length-1][0];
    head_x_right = &snake_x[current_length-1][1];
    head_y_right = &snake_y[current_length-1][1];
    
    //actualizar pointers de la cola
    tail_x_left = &snake_x[0][0]; //la cola siempre va en la primera posición
    tail_y_left = &snake_y[0][0];
    tail_x_right = &snake_x[0][1];
    tail_y_right = &snake_y[0][1];
}

//checho el color del pixel
unsigned int get_pixel_color(unsigned int x, unsigned int y) {
    unsigned int *led_base = LED_MATRIX_0_BASE;
    unsigned int offset = x + (24-y) * LED_MATRIX_0_WIDTH; //calcular offset para el pixel especifico
    return *(led_base + offset); //el valor del apuntador a nuestro led contiene el valor del color
}

int check_border_collision() {
    //color de los píxeles donde está la cabeza
    unsigned int color_left = get_pixel_color(*head_x_left, *head_y_left);
    unsigned int color_right = get_pixel_color(*head_x_right, *head_y_right);
    
    //si es rojo es que chocó con un borde
    if (color_left == border_color || color_right == border_color) {
        return 1;
    }
    return 0;
}

void game_over() {
    clear();

    printf("Game over xd\n");  
    printf("                     __\n");  
    printf("        _______     /x_>-<\n");  
    printf("    ___/ _____ \\__/ /\n");  
    printf(" <<<____/     \\____/\n");  
    printf("- Score: %d", score);  

    while(1) {} //paramos el loop
}

void movimiento(unsigned int mov) {
    //guardamos la posición original de la cola antes de moverla
    unsigned int old_tail_x_left = *tail_x_left;
    unsigned int old_tail_y_left = *tail_y_left;
    unsigned int old_tail_x_right = *tail_x_right;
    unsigned int old_tail_y_right = *tail_y_right;
    
    //movemos cada segmento una posición
    for(int i = 0; i < current_length-1; i++) {
        snake_x[i][0] = snake_x[i+1][0];
        snake_y[i][0] = snake_y[i+1][0];
        snake_x[i][1] = snake_x[i+1][1];
        snake_y[i][1] = snake_y[i+1][1];
    }
    
    //movemos head segun la dirección
    switch(mov) {
        ////////////////////////// EJE Y: /////////////////////////
        case 1: //arriba
            if (!vertical_orientation) {//si horizontal
                vertical_orientation = 1; //cambia  vertical
                //cambiar pixel derecho
                *head_x_right = *head_x_left + 1;
                *head_y_right = *head_y_left;
            }
            //mover ambos hacua arriba
            (*head_y_left)++;
            (*head_y_right)++;
            break;
            
        case 2: //abajo
            if (!vertical_orientation) {//si hroizontal
                vertical_orientation = 1;//cambia vertical
                //cambiar pixel derecho
                *head_x_right = *head_x_left + 1;
                *head_y_right = *head_y_left;
            }
            //ambos hacia abajo
            (*head_y_left)--;
            (*head_y_right)--;
            break;
        
        /////////////////////////// EJE X: //////////////////////////
        case 3: //izquierda
            if (vertical_orientation) {//si hvertical
                vertical_orientation = 0; //cambia horizontal
                //mover pixel drrecho de la cabeza
                *head_x_right = *head_x_left;
                *head_y_right = *head_y_left + 1;
            }
            //ambos pixeles hacia la izquierda
            (*head_x_left)--;
            (*head_x_right)--;
            break;
            
        case 4: //derecha
            if (vertical_orientation) {//si vertical
                vertical_orientation = 0; //cambia horizontal
                //mover pixel derecho
                *head_x_right = *head_x_left;
                *head_y_right = *head_y_left + 1;
            }
            //ambos hacia la derecha
            (*head_x_left)++;
            (*head_x_right)++;
            break;
    }
    
    // Verificar colisión con bordes antes de actualizar
    if (check_border_collision()) {
        game_over();
        return;
    }
    
    //////////////////////////// Efecto de movimiento ///////////////////////
    //borramos antigua posición de la cola:
    set_pixel(old_tail_x_left, old_tail_y_left, 0);
    set_pixel(old_tail_x_right, old_tail_y_right, 0);
    //dibujar nueva posición de la cabeza
    set_pixel(*head_x_left, *head_y_left, snake_color);
    set_pixel(*head_x_right, *head_y_right, snake_color);
}

void spawn_snake() { //nomas recoloreamos la serpiente de verde
    for(int i = 0; i < current_length; i++) {
        set_pixel(snake_x[i][0], snake_y[i][0], snake_color);
        set_pixel(snake_x[i][1], snake_y[i][1], snake_color);
    }
}

void spawn_apple() {
    //generamos coordenadas entre 1 y max-1 para evitar los bordes y que se salga
    ax = 1 + (rand() % (max_x - 1));
    ay = 1 + (rand() % (max_y - 1));

    printf("Manzana en: %d, %d\n", ax, ay); //traza
    
    //spawneamos 2x2 manualmente
    set_pixel(ax, ay, apple_color);
    set_pixel(ax+1, ay, apple_color);
    set_pixel(ax, ay+1, apple_color);
    set_pixel(ax+1, ay+1, apple_color);
}

//calculamos con el offset el pixel y le seteamos el color, gardando el hex de este en el apuntador del pixel
void set_pixel(unsigned int x, unsigned int y, unsigned int color) {
    unsigned int *led_base = LED_MATRIX_0_BASE;
    unsigned int *address;
    unsigned int offset;

    offset = x + (24-y) * LED_MATRIX_0_WIDTH; //NOTA: Hacer una función que calcule el offset
    address = led_base + offset;
    *(address) = color;
}

void delay() { //hacer tiempo con un loop vacio
    for (volatile int i = 0; i < speed; i++) {}
}
