#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char ticks;
unsigned char microticks;
unsigned char end_of_blink;
unsigned char end_of_pause;
unsigned short stream_index;
unsigned char current_byte;
unsigned char bit_mask;
unsigned char rn_q;
unsigned char rn_g;
unsigned char rn_x;
unsigned char rn_a;
unsigned char rn_b;

//#define rn_hi 1
//#define rn_nu 2
#define rn_Ha 2
#define rn_Hb 2

#define PULSE_DURATION 42
#define PULSE_BURST 1
#define LEADING_BURST (PULSE_BURST * 16)
#define LEADING_AFTERBURST (LEADING_BURST / 2)
#define FULL_LENGTH 192 * PULSE_BURST
#define DATA_LENGTH (PULSE_BURST * 120)
#define FLAG_LENGTH 32
#define FLAG_PACKETS 178
#define BYTES_PER_PACKET 4

unsigned char flag_prefix[] = {'R','u','C','T','F','_'};

unsigned char rol(unsigned char value, unsigned char shift)
{
    while (shift--)
    {
        value = (value << 1) | (value >> 7);
    }
    return value;
}

//1 pulse = 1/38000 / 2
//1 burst = 42/38000 / 2
//1 byte = 192 * 42 / 38000 / 2

ISR(TIM0_COMPA_vect, ISR_NAKED)
{
    if (ticks < end_of_blink)
    {
        PORTB ^= (1 << PB3);
    }
    if (microticks == 0)
    {
        if (ticks == 0)
        {
            end_of_blink = LEADING_BURST;
            end_of_pause = LEADING_BURST + LEADING_AFTERBURST;
        }
        if (ticks == end_of_blink)
        {
            PORTB &= ~(1 << PB3);
        }
        else if (ticks == end_of_pause)
        {
            if (ticks == DATA_LENGTH)
            {
                end_of_pause = FULL_LENGTH - DATA_LENGTH;
            }
            else
            {
                if (bit_mask == 0)
                {
                    if ((stream_index & 3) == 0)
                    {
                        unsigned char temp = stream_index >> 2;
                        if (temp < sizeof(flag_prefix))
                        {
                            current_byte = flag_prefix[temp];
                        }
                        else if (temp < FLAG_LENGTH - 1)
                        {
                            unsigned char z = rn_g;
                            unsigned char r = rn_q;
                            rn_q = z ^ rol(rn_x, rn_hi);
                            rn_g = r ^ rol(z, rn_nu);
                            rn_x = rn_a * z + rn_b;
                            rn_a += rn_Ha;
                            rn_b += rn_Hb;
                            current_byte = r & 15;
                            if (current_byte >= 10)
                            {
                                current_byte += 55;
                            }
                            else
                            {
                                current_byte += 48;
                            }
                        }
                        else if (temp == FLAG_LENGTH - 1)
                        {
                            current_byte = '=';
                        }
                        else
                        {
                            current_byte = 0;
                        }
                    }
                    else
                    {
                        current_byte = ~current_byte;
                    }
                    bit_mask = 1;
                    if (++stream_index == FLAG_PACKETS * BYTES_PER_PACKET)
                    {
                        stream_index = 0;
                    }
                }
                if (!(current_byte & bit_mask))
                {
                    end_of_pause = 2 * PULSE_BURST;
                }
                else
                {
                    end_of_pause = 4 * PULSE_BURST;
                }
                bit_mask <<= 1;
            }
            end_of_blink = PULSE_BURST;
            end_of_blink += ticks;
            end_of_pause += ticks;
        }
    }
    if (++microticks == PULSE_DURATION)
    {
        if (++ticks == FULL_LENGTH)
        {
            ticks = 0;
        }
        microticks = 0;
    }
    reti();
}

int main()
{
    ticks = 0;
    microticks = 0;
    stream_index = 0;
    end_of_blink = 0;
    rn_q = 0;
    rn_g = 0;
    rn_x = 0;
    rn_a = 1;
    rn_b = 1;
    DDRB |= (1 << PB3);
    PORTB = 0;
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1<<CS00);
    TIMSK0 |= 1 << OCIE0A;
    OCR0A = 59;
    sei();
    for(;;);
}