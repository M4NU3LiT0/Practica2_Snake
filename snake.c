#include "ripes_system.h"
#include <stdio.h>

#define W LED_MATRIX_0_WIDTH //width
#define H LED_MATRIX_0_HEIGHT //height 
#define L (H*W)

// pointer a las leds
volatile unsigned int *led_base = LED_MATRIX_0_BASE;
volatile unsigned int *snake[L];  //array de la serpiente
int snakeLenght = 8;

void bordes() { 
    // horizontales
    for (int i = 0; i < W; i++) {
        *(led_base + i) = 0xFF0000;               //  borded e arriba
        *(led_base + (W * (H - 1)) + i) = 0xFF0000;// borde de abajo
    }
    //verticales
    for (int i = 0; i < H; i++) {
        *(led_base + (W * i)) = 0xFF0000;         // izquierda
        *((led_base + (W * i)) + (W - 1)) = 0xFF0000; // derecha
    }
}

void clear() {
    for (int i = 0; i < (W * H); i++) {
        *(led_base + i) = 0x00;
    }
}

void spawnSnake() {
    //primera fila:
    snake[0] = led_base + W +2;
    snake[1] = led_base + W +3;
    snake[2] = led_base + W +4;
    snake[3] = led_base + W +5;
     //segunda fila:
     snake[4] = led_base + (2 * W) + 2;
    snake[5] = led_base + (2 * W) + 3;
    snake[6] = led_base + (2 * W) + 4;
    snake[7] = led_base + (2 * W) + 5;
     
     
    //color - verde
    for (int i=0; i<snakeLenght; i++) {
        *snake[i] = 0x00FF00;
    }
}

int main() {
    clear();
    bordes();
    spawnSnake();
    while(1) {
        ///
    }
    return 0;
}
