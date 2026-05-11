#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t tickCounter = 0;

/* 
  Internal clock is 1mHz. Since the prescaler is set to 1024, TCNT0 increments 1,000,000Hz / 1024 = 976Hz.
  So in 1 second, TCNT increments 976 times per second. To blink every half second, we want a toggle every 488 
  increments. so we set OCR0A to 244, so every 2 interrupts is a toggle. 
*/
ISR(TIMER0_COMPA_vect)  // Will interrupt everytime TCNT0 == OCR0A
{
  tickCounter = (tickCounter + 1) % 2;  // Toggle tickCounter between 0 and 1
}

int main()
{
  DDRD = (1 << PD7);   // Set PD7 as output

  /* Sets bit 1. Mode: CTC. This makes the interrupt trigger once the counter reaches the number in OCR0A */
  TCCR0A = (1 << WGM01);   
  /* TCCR0B sets prescaler. ...101 sets it to 1024, meaning every 1024 clock cycles, the TCNT0 will increment. */
  TCCR0B = (1 << CS02) | (1 << CS00);  

  OCR0A = 243;              // Interrupt will occur every 244 counter increments

  TIMSK0 = (1 << OCIE0A);  // Enable timer interrupts from OCR0A == TCNT0

  sei();

  while(1)
  {
    if (tickCounter)
    {
      PORTD ^= (1 << PD7);  // Toggle PD7
    }
  }
}