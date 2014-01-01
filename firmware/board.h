void reset_cpu(void);

/* LED */
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

/* SPI */
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_SS   PB2
#define SPI_SCK  PB5
#define SPI_MOSI PB3
#define SPI_MISO PB4

/* nRF24L01+ SPI */
#define NRF_DDR  DDRB
#define NRF_PORT PORTB
#define NRF_CS   SPI_SS
#define NRF_CE   PB1

#define nrf_cs_h() (NRF_PORT |=  _BV(NRF_CS))
#define nrf_cs_l() (NRF_PORT &= ~_BV(NRF_CS))
#define nrf_ce_h() (NRF_PORT |=  _BV(NRF_CE))
#define nrf_ce_l() (NRF_PORT &= ~_BV(NRF_CE))
