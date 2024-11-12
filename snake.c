#include "ripes_system.h"
#include <stdio.h>
#define W LED_MATRIX_0_WIDTH
#define H LED_MATRIX_0_HEIGHT
#define L (H*W)

volatile unsigned int *led_base = LED_MATRIX_0_BASE;
volatile unsigned int *snake[L];  // array de la serpiente
int snakeLenght = 8; 
int pos_x = 1;
int pos_y = 1;

void bordes() { 
    for (int i = 0; i < W; i++) {
        *(led_base + i) = 0xFF0000; //arriba
        *(led_base + (W * (H - 1)) + i) = 0xFF0000; //abajo
    }
    for (int i = 0; i < H; i++) {
        *(led_base + (W * i)) = 0xFF0000; //izquierda
        *((led_base + (W * i)) + (W - 1)) = 0xFF0000; //derecha
    }
}

void clear() {
    for (int i = 0; i < (W * H); i++) {
        *(led_base + i) = 0x00;
    }
}

void spawnSnake() {
    // Primera fila (4) --- snake[0] = led_base + W + 2;
    int limit = snakeLenght / 2; 
    for (int i = 0; i < limit; i++) {
        snake[i] = led_base + W + (i + 2);
    }
    // Segunda fila (4) --- snake[4] = led_base + (2 * W) + 2;
    for (int i = 0; i < limit; i++) {
        snake[i + limit] = led_base + (2 * W) + (i + 2);//corregido xd
    }
    
    
    // Color - verde
    for (int i = 0; i < snakeLenght; i++) {
        *snake[i] = 0x00FF00;
    }
}

void moveDerecha() {
    // Limpiar la cola:
    *snake[0] = 0x000000;
    *snake[snakeLenght/2] = 0x000000;
    
    // Mover primera fila:
    for (int i = 0; i < (snakeLenght/2) - 1; i++) {
        snake[i] = snake[i + 1];
    }
    snake[(snakeLenght/2) - 1] = snake[(snakeLenght/2) - 2] + 1;
    
    // Mover segunda fila:
    for (int i = snakeLenght/2; i < snakeLenght - 1; i++) {
        snake[i] = snake[i + 1];
    }
    snake[snakeLenght - 1] = snake[snakeLenght - 2] + 1;
    
    //repintamos
    for (int i = 0; i < snakeLenght; i++) {
        *snake[i] = 0x00FF00;
    }
}

void delay(int millis) {
    for (volatile int i = 0; i < millis * 1000; i++) {
    }
}

int main() {
    clear();
    bordes();
    spawnSnake();
    while (1) {
        moveDerecha();
        delay(5);
    }
    return 0;
}
