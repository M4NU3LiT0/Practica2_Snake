
#include "ripes_system.h"
#define SW0 (0x01)
#define SW1 (0x02)
#define SW2 (0x04)
#define SW3 (0x08)
volatile unsigned int * led_base = LED_MATRIX_0_BASE;
volatile unsigned int * switch_base = SWITCHES_0_BASE;
volatile unsigned int * DPad_Left = D_PAD_0_LEFT;
volatile unsigned int * DPad_Right = D_PAD_0_RIGHT;
volatile unsigned int * DPad_Down = D_PAD_0_DOWN;
volatile unsigned int * DPad_Up = D_PAD_0_UP;
void main()
{
//RRGGBB
int color0 = 0x0000FF; // Blue color for LEDs
int color1 = 0x00FF00; // Green color for LEDs
int color2 = 0xFF0000; // Red color for LEDs
int color3 = 0xFFFFFF; // White color for LEDs
while(1)
	{
	if(*DPad_Left)
		led_base--;
	if(*DPad_Right)
		led_base++;
	if(*DPad_Down)
		led_base += LED_MATRIX_0_WIDTH;
	if(*DPad_Up)
		led_base -= LED_MATRIX_0_WIDTH;
	if(*switch_base & SW0) // Get the value of SW0
		*led_base = color0;
	if(*switch_base & SW1) // Get the value of SW1
		*led_base = color1;
	if(*switch_base & SW2) // Get the value of SW2
		*led_base = color2;
	if(*switch_base & SW3) // Get the value of SW3
		*led_base = color3;
}
}
