/*
 * Copyright 2013 Fabio Baltieri (fabio.baltieri@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "board.h"
#include "id.h"
#include "adc.h"
#include "spi.h"
#include "nrf24l01p.h"
#include "nrf_frames.h"

#define div_round(a, b) (((a) + ((b)/2)) / (b))

static struct nrf_frame frm;

void reset_cpu(void)
{
	wdt_disable();

	wdt_enable(WDTO_15MS);

	for (;;);
}

static void hello(void)
{
	uint8_t i;

	for (i = 0; i < 8; i++) {
		led_toggle();
		_delay_ms(50);
	}
}

static void fill_frame(struct nrf_frame *frm)
{
	struct nrf_ambient *na = &frm->msg.ambient;
	uint32_t val;

	memset(frm, 0, sizeof(*frm));

	frm->board_id = get_board_id();
	frm->msg_id = NRF_MSG_ID_AMBIENT;
	frm->len = sizeof(*frm);
	frm->seq = 0;

	/* Vbatt */
	val = adc_get(ADC_VBATT, 1);
	val = div_round(val * ADC_VREF_mV * VBAT_H,
			ADC_VREF_BITS * VBAT_L);
	na->vbatt = val;

	if (is_charging())
		na->vbatt |= NRF_POWER_VBATT_CHARGING;
}

ISR(PCINT0_vect)
{
	nrf_poll();

	led_off();
}

ISR(WDT_vect)
{
	led_on();

	fill_frame(&frm);

	nrf_standby();
	nrf_tx((uint8_t *)&frm, sizeof(frm));
}

int __attribute__((noreturn)) main(void)
{
	led_init();
	spi_init();
	nrf_init();
	adc_init();
	chg_init();
	read_board_id();

	/* nRF interrupt */
	NRF_PCMSK |= _BV(NRF_PCINT);
	PCICR |= _BV(PCIE0);

	/* WDT as periodic interrupt */
	WDTCSR |= (1 << WDE) | (1 << WDCE);
	WDTCSR = (0 << WDCE) | (1 << WDIF) | (1 << WDIE) |
		(1 << WDP2) | (0 << WDP1) | (1 << WDP0);

	hello();
	led_off();

	sei();
	for (;;) {
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
	}
}
