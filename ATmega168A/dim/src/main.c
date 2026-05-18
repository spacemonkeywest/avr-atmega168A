#include <avr/io.h>

int main()
{
    // PD5 and PD6 correspond to timer0 pwm output pins. Will set those to output.
    DDRD = (1 << PD6);

    // TCCR0A bits 2:0 set mode. Will set to Fast PWM mode
    TCCR0A = (1 << WGM00) | (1 << WGM01);

    // TCCR0A bits 7:6 set the behaviour for OC0A (PD6). Will set to non-inverting
    TCCR0A |= (1 << COM0A1);

    // TCCR0B bits 2:0 set the clock prescaler. Will set to 1
    TCCR0B = (1 << CS00);

    // For Fast PWM mode and non-inverting, if TCNT0 < OCR0A, the pin goes HIGH
    // Setting to 64 makes the pin HIGH 1/4 of the time, so it looks 1/4 of max brightness
    OCR0A = 64;

    while(1);
}