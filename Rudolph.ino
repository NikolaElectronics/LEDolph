#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

const byte ledPin0 = 8;  // Define the pin for the LED
int fadeValue;            // Variable to store the PWM value for fading
byte saveADCSRA;          // Variable to save the state of ADCSRA
volatile byte counterWD = 0;  // Counter for Watchdog Timer interrupts

void setup()
{
  resetWatchDog();        // Initialize and configure the Watchdog Timer
  pinMode(ledPin0, OUTPUT);  // Set the LED pin as an output
  // Blink the LED to indicate setup
  digitalWrite(ledPin0, HIGH);
  delay(200);
  digitalWrite(ledPin0, LOW);
  delay(200);
  digitalWrite(ledPin0, HIGH);
  delay(200);
  digitalWrite(ledPin0, LOW);
  delay(200);
  led0Low();  // Start with a fading effect
}

void loop()
{
  if (counterWD == 5)
  {
    led0High();          // Fade in the LED
    led0Low();           // Fade out the LED
    analogWrite(ledPin0, LOW);  // Turn off the LED
    delay(2000);         // Wait for 2 seconds
    counterWD = 0;       // Reset the counter
  }
  sleepNow();            // Put the microcontroller to sleep
}

void sleepNow()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Set sleep mode to Power Down
  saveADCSRA = ADCSRA;  // Save the state of ADCSRA
  ADCSRA = 0;           // Disable the ADC to save power
  power_all_disable();   // Disable all modules to save power
  noInterrupts();        // Disable interrupts
  resetWatchDog();       // Reset the Watchdog Timer
  sleep_enable();        // Enable sleep mode
  interrupts();          // Enable interrupts
  sleep_cpu();           // Put the CPU to sleep
  sleep_disable();       // Disable sleep mode
  power_all_enable();    // Enable all modules after waking up
}

void resetWatchDog()
{
  MCUSR = 0;  // Clear the MCU status register
  // Enable the Watchdog Timer and set the timeout period
  WDTCSR = bit(WDCE) | bit(WDE) | bit(WDIF);
  WDTCSR = bit(WDIE) | bit(WDP2) | bit(WDP1);
  wdt_reset();  // Reset the Watchdog Timer
}

ISR(WDT_vect)
{
  wdt_disable();  // Disable the Watchdog Timer during ISR
  counterWD++;    // Increment the counter on Watchdog Timer interrupt
}

void led0High()
{
  for (fadeValue = 0; fadeValue <= 255; fadeValue += 5)
  {
    analogWrite(ledPin0, fadeValue);  // PWM to fade in the LED
    delay(50);
  }
}

void led0Low()
{
  for (fadeValue = 255; fadeValue >= 0; fadeValue -= 5)
  {
    analogWrite(ledPin0, fadeValue);  // PWM to fade out the LED
    delay(50);
  }
}
