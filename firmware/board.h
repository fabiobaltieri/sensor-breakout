/* LEDs */

#define LED_PORT	PORTD
#define LED_DDR		DDRD
#define LED		PD6

#define led_on()	LED_PORT |=  _BV(LED)
#define led_off()	LED_PORT &= ~_BV(LED)
#define led_toggle()	LED_PORT ^=  _BV(LED)

#define led_init()				\
	do {					\
		LED_DDR |= _BV(LED);		\
		LED_DDR |= _BV(LED);		\
	} while (0);
