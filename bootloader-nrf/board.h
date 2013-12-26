void reset_cpu(void);

/* LED */
#define LED_PORT	PORTC
#define LED_DDR		DDRC
#define LED		PC6

#define led_on()	LED_PORT |=  _BV(LED)
#define led_off()	LED_PORT &= ~_BV(LED)
#define led_toggle()	LED_PORT ^=  _BV(LED)

#define led_init()				\
	do {					\
		LED_DDR |= _BV(LED);		\
		LED_DDR |= _BV(LED);		\
	} while (0);

/* SPI */
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_SS   PB0
#define SPI_SCK  PB1
#define SPI_MOSI PB2
#define SPI_MISO PB3

/* nRF24L01+ SPI */
#define NRF_DDR  DDRB
#define NRF_PORT PORTB
#define NRF_CS   SPI_SS
#define NRF_CE   PB4

#define nrf_cs_h() (NRF_PORT |=  _BV(NRF_CS))
#define nrf_cs_l() (NRF_PORT &= ~_BV(NRF_CS))
#define nrf_ce_h() (NRF_PORT |=  _BV(NRF_CE))
#define nrf_ce_l() (NRF_PORT &= ~_BV(NRF_CE))
