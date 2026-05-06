#include <avr/io.h>
#include <avr/interrupt.h>

#define DEBOUNCE_THRESHOLD 500  // Number of loops to consider for debouncing
#define RESET_THRESHOLD 10000   // Number of loops to consider for resetting state after holding the button

volatile uint8_t currentState = 0;        // OFF -> ON -> SLOW -> FAST
volatile uint16_t count = 0;              // Will keep track of time spent while on/off during SLOW and FAST
volatile uint8_t pendingStateChange = 0;  // Flag to indicate if a state change is pending (used to debounce the button)
uint8_t debouncing = 0;                   // Flag to indicate if we are currently debouncing
uint16_t debounceCounter = 0;             // Counter for debouncing
uint16_t resetCounter = 0;                // Counter to reset state after 2 second hold



ISR(PCINT0_vect)
{
  if (PINB & (1 << PB0))    // Only increase state if its a push, not a release
  {
    count = 0;
    pendingStateChange = 1;
  }
}

int main()
{
  DDRD |= (1 << PD6);       // PD6 as output

  DDRB &= ~(1 << PB0);      // PB0 as input
  PORTB &= ~(1 << PB0);     // Disable internal pullup resistor

  // Set up interrupts

  PCICR |= (1 << PCIE0);    // Enable pin change interrupts for PORT B
  PCMSK0 |= (1 << PCINT0);  // Enable pin change interrupt for PB0

  sei();


  while(1)
  {

    // ***** Reset check *****
    if (PINB & (1 << PB0))
    {
      resetCounter++;

      if (resetCounter > RESET_THRESHOLD)
      {
        currentState = 0;
        resetCounter = 0;
      }
    }
    
    if (!(PINB & (1 << PB0)))
    {
      resetCounter = 0;
    }

    // ***** Handle debouncing *****
    if (pendingStateChange && !debouncing)
    {
      debouncing = 1;
      debounceCounter = 0;
      pendingStateChange = 0;
    }

    if (debouncing)
    {
      debounceCounter++;
      if (debounceCounter >= DEBOUNCE_THRESHOLD)
      {
        debouncing = 0;
        debounceCounter = 0;
        if (PINB & (1 << PB0))  // If the button is still pressed after debounce time, we consider it a valid press
        {
          currentState++;
          currentState %= 4;
        }
      }
    }

    // ***** Update LED state *****
    switch (currentState)
    {
    case 0:                     // OFF
      PORTD &= ~(1 << PD6);
      break;
    
    case 1:                     // ON
      PORTD |= (1 << PD6);
      break;

    case 2:                     // SLOW blinks, every 50000 loops
      count++;
      count %= 50000;           // Reset count every 50000 loops to create a blinking pattern
      if (count < 25000)        // LED on for the first half of the cycle, off for the second half
      {
        PORTD &= ~(1 << PD6);
      }
      else
      {
        PORTD |= (1 << PD6);
      }
      break;

    case 3:                     // FAST blinks, every 5000 loops
      count++;
      count %= 5000;            // Reset count every 5000 loops to create a blinking pattern
      if (count < 2500)         // LED on for the first half of the cycle, off for the second half
      {
        PORTD &= ~(1 << PD6);
      }
      else
      {
        PORTD |= (1 << PD6);
      }
      break;

    default:
      break;
    }
  }

  return 0;
}