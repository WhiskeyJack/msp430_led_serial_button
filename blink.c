#include  <msp430.h>
#include  <serial.h>

// Define meaningful names for different bits
#define LED_RED BIT0
#define LED_GREEN BIT6
#define LED_OUT P1OUT
#define BUTTON_1 BIT3

// keep track of blink state
unsigned int blink_state = 0;
unsigned int new_blink_state = 0;

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer. This line of code is needed at the beginning of most MSP430 projects.
                                            // This line of code turns off the watchdog timer, which can reset the device after a certain period of time.

  P1DIR |= (LED_RED | LED_GREEN);			// BIT0/LED_RED (0x01) is red led, BIT6/LED_GREEN (0x40) is green led
   	   	   	   	   	   	   	   	   	   	    // We set both bits because we want to control both the red and green led
											// P1DIR is a register that configures the direction (DIR) of a port pin as an output or an input.
  	  	  	  	  	  	  	  	  	  	    // To set a specific pin as output or input, we write a '1' or '0' on the appropriate bit of the register.
  	  	  	  	  	  	  	  	  	  	    // (BIT0 | BIT6) sets bit0 and bit6 to 1 (bitwise adding 1 and 1 or 1 and 0 add up to 1)
  	  	  	  	  	  	  	  	  	  	  	// (BIT0 | BIT6) equals 00000001 + 01000000 = 01000001 ( => 0x01 + 0x40 = 0x41)
  	  	  	  	  	  	  	  	  	  	  	// P1DIR = <PIN7><PIN6><PIN5><PIN4><PIN3><PIN2><PIN1><PIN0>

  P1IE |= BUTTON_1; 					    // Enable button P1.3 interrupt
  P1IFG &= ~BUTTON_1; 						// P1.3 IFG (Interrupt flag) cleared

  __enable_interrupt(); 					// enable all interrupts (also done in serial.h)

  //LED_OUT = LED_RED;						// Since we want to blink the on-board red LED, we want to set the direction of Port 1, Pin 0 (P1.0) as an output
                                            // We do that by writing a 1 on the PIN0 bit of the P1DIR register
                                            // P1DIR = <PIN7><PIN6><PIN5><PIN4><PIN3><PIN2><PIN1><PIN0>
                                            // P1DIR = 0000 0001
                                            // P1DIR = 0x01     <-- this is the hexadecimal conversion of 0000 0001
  	  	  	  	  	  	  	  	  	  	  	// Because we want to alternate, we only set one of the leds in the on position (the red one)

  LED_OUT = ~(LED_RED | LED_GREEN);			// Turn both leds off

  Initialize_Serial();						// Initialize serial communication (in serial.h)

  Serial_Write("Start, state: Off\r\n");

  for (;;)                                  // This empty for-loop will cause the lines of code within to loop infinitely
  {


	  //LED_OUT ^= (LED_RED | LED_GREEN);     // Toggle P1.0 using exclusive-OR operation (^=)
                                            // P1OUT is another register which holds the status of the LED.
                                            // '1' specifies that it's ON or HIGH, while '0' specifies that it's OFF or LOW
                                            // Since our LED is tied to P1.0, we will toggle the 0 bit of the P1OUT register
	  	  	  	  	  	  	  	  	  	  	// Since we XOR P1OUT with both led bits and we only set the red one before this will flip the bits of both leds to alternate states
	  	  	  	  	  	  	  	  	  	    // XOR toggles a bit and when both are 1 it will result in zero (as opposed to OR)
	  	  	  	  	  	  	  	  	  	  	// P1OUT ^= (BIT0 | BIT6) => 00000001 ^= 01000001 = 01000000

	  if (blink_state != new_blink_state) {			// check if the button was pressed, if so new_blink_state will be different from blink_state
		  switch (new_blink_state) {
		  case 0:
			  LED_OUT = ~(LED_RED | LED_GREEN);
			  Serial_Write("State: Off\r\n");
			  break;
		  case 1:
			  LED_OUT = LED_RED;
			  Serial_Write("State: Red\r\n");
			  break;
		  case 2:
			  LED_OUT = LED_GREEN;
			  Serial_Write("State: Green\r\n");
			  break;
		  case 3:
			  LED_OUT = (LED_RED | LED_GREEN);
			  Serial_Write("State: Simultaneous\r\n");
			  break;
		  case 4:
			  LED_OUT = LED_GREEN;
			  Serial_Write("State: Alternate\r\n");
			  break;
		  default:
			  LED_OUT = ~(LED_RED | LED_GREEN);
			  Serial_Write("State: Off\r\n");
		  }
		  blink_state = new_blink_state;		// reset blink_state to new_blink_state
	  }

	  if (blink_state ==3 || blink_state == 4)	// only blink states 3 and for require blinking
		  LED_OUT ^= (LED_RED | LED_GREEN);		// toggle leds with an exclusive-OR operation (^=)

	  _delay_cycles(500000);                	// Delay between LED toggles (blinking speed).

  }
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if (new_blink_state > 3)
		new_blink_state = 0;
	else
		new_blink_state += 1;

	P1IFG &= ~BUTTON_1; // P1.3 IFG cleared
	//P1IES ^= BUTTON_1; // toggle the interrupt edge,
}
