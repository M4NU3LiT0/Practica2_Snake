#include "ripes_system.h"
#include <stdio.h>

#define W LED_MATRIX_0_WIDTH
#define H LED_MATRIX_0_HEIGHT

// pointer a las leds
volatile unsigned int *led_base = LED_MATRIX_0_BASE;

void bordes() { 
    // horizontales
    for (int i = 0; i < W; i++) {
        *(led_base + i) = 0xFF0000;               //  borded e arriba
        *(led_base + (W * (H - 1)) + i) = 0xFF0000; // borde de abajo
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

int main() {
    clear();
    bordes();
    while(1) {
        ///
    }
    return 0;
}
