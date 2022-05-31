#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>


// initialize pwm
void pwm_init() {
    /*set prescaler to /8
      fast PWM modde
      enable PB1
      disable PB0
     */
    TCCR0A = 0x00;
    TCCR0A = (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
    TCCR0B = 0x00;
    TCCR0B = (1<<WGM02)|(1<<CS00);

    TCNT0 = 0;

    // F_CPU / 8 / (39 + 1) = 25khz 
    OCR0A = 39;
    // Initial duty cycle
    OCR0B = 9;
}

// initialize adc
void adc_init()
{
    // AREF = AVcc
    // Enable channel 2
    ADMUX = (1<<MUX1)|(1<<ADLAR);
 
    // ADC Enable and prescaler of 128
    // Enable conversion interrupt
    // 1000000/8 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADIE);
}

// read adc value
void adc_start_conversion()
{
    // start single conversion
    // write '1' to ADSC
    ADCSRA |= (1<<ADSC);
}
 
// initialize timer
void timer_init()
{
    TCCR1 = 0; // Stop the timer
    TCNT1 = 0; // Zero the timer
    GTCCR = (1<<PSR1); // reset the prescaler
    OCR1A = 25; // set the comparison value
    OCR1C = 25;
    TIMSK = (1<<OCIE1A); // enable a interrupt
    TCCR1 = (1<<CTC1)|(1<<CS13)|(1<<CS12)|(1<<CS11)|(1<<CS00);

    sei();
}

ISR(TIM1_COMPA_vect)
{
    MCUCR &= ~(1<<SE); // disable sleep mode
    adc_start_conversion();
}

ISR(ADC_vect) 
{
    // write scaled value into PWM register
    OCR0B = 10 + ADCH / 9;
}

void enter_sleep()
{  
  MCUCR &= ~((1<<SM1)|(1<<SM0));      // enabling sleep mode and powerdown sleep mode
  MCUCR |= (1<<SE);     // enabling sleep enable bit
  __asm__ __volatile__ ( "sleep" "\n\t" :: ); //Sleep instruction to put controller to sleep
  // controller stops executing instruction after entering sleep mode  
}

int main()
{
    DDRB = (1<<DDB1);

    // initialize adc and pwm
    adc_init();
    pwm_init();
    timer_init();

    while(1)
    {
        enter_sleep();
    }
}
