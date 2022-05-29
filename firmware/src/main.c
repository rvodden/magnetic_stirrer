#include <avr/io.h>
#include <avr/interrupt.h>
 
// initialize pwm
void pwm_init() {
    /*set prescaler to /8
      fast PWM modde
      enable PB1
      diable PB0
     */
    TCCR0A = (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
    TCCR0B = (1<<WGM02)|(1<<CS00);

    // F_CPU / 8 / (39 + 1) = 25khz 
    OCR0A = 39;
    // Initial duty cycle
    OCR0B = 20;
}

// initialize adc
void adc_init()
{
    // AREF = AVcc
    ADMUX = (1<<REFS0);
 
    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

// read adc value
uint16_t adc_read(uint8_t channel)
{
    // select the corresponding channel 0~7
    // ANDing with '7' will always keep the value
    // of 'ch' between 0 and 7
    channel &= 0x07;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8) | channel;     // clears the bottom 3 bits before ORing
 
    // start single conversion
    // write '1' to ADSC
    ADCSRA |= (1<<ADSC);
 
    // wait for conversion to complete
    // ADSC becomes '0' again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));
 
    return (ADC);
}
 
// initialize timer
void timer_init()
{
    TCCR1 = 0; // Stop the timer
    TCNT1 = 0; // Zero the timer
    GTCCR = (1<<PSR1); // reset the prescaler
    OCR1A = 243; // set the comparison value
    OCR1C = 243;
    TIMSK = (1<<OCIE1A); // enable a interrupt
    TCCR1 = (1<<CTC1)|(1<<CS13)|(1<<CS12)|(1<<CS11)|(1<<CS10);

    sei();
}

ISR(TIM1_COMPA_vect)
{
    MCUCR &= ~(1<<SE); // disable sleep mode
    uint16_t adc_result;
    adc_result = adc_read(0);      // read adc value at PB5
    OCR0B = 10 + adc_result / 9; 
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
    DDRB = (1<<PB1);
 
    // initialize adc and pwm
    adc_init();
    pwm_init();
    timer_init();

    while(1)
    {
        enter_sleep();
    }
}