#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t button_changed = 0;   // Flag for indicating button click

ISR(PCINT0_vect)
{
  button_changed = 1;
}

int main()
{
  DDRD |= (1 << PD6);         // Output

  DDRB &= ~(1 << PB0);        // Input
  PORTB &= ~(1 << PB0);       // Disable pullup resistor. Using external pulldown

  PCICR |= (1 << PCIE0);      // Enable pin change interrupt for PORTB
  PCMSK0 |= (1 << PCINT0);    // Enable pin change interrupt for PB0

  sei();                      // Enable global interrupts

  uint8_t last_state = 0;     // Last state of the button

  while (1) {
    if (button_changed) {
        button_changed = 0;
        
        _delay_ms(50);
        
        uint8_t current_state = (PINB & (1 << PB0)) ? 1 : 0;
        
        if (current_state == 1 && last_state == 0) {  // Rising edge (press)
            PORTD ^= (1 << PD6);  // Toggle LED
        }
        
        last_state = current_state;
    }
}

  return 0;
}