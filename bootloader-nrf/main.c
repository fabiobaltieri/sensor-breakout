/*
 * Copyright 2013 Fabio Baltieri (fabio.baltieri@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "board.h"
#include "nrf_boot.h"
#include "spi.h"
#include "nrf24l01p.h"

void reset_cpu(void)
{
	wdt_disable();

	wdt_enable(WDTO_15MS);

	for (;;);
}

static void (*run_payload)(void) = 0;

static void leave_bootloader(void)
{
	wdt_reset();

	cli();

	/* clean SPI config */
	SPCR = 0;
	SPSR = 0;

	/* move interrupt vectors back to the application code */
	MCUCR = 1 << IVCE;
	MCUCR = 0 << IVSEL;

	run_payload();
}

int __attribute__((noreturn)) main(void)
{
	volatile int zero = 0;
	uint8_t mcusr;

	mcusr = MCUSR;
	MCUSR = 0;

	led_init();
	spi_init();
	nrf_init();

	wdt_enable(WDTO_2S);

	led_on();

	/* move interrupt vectors to the boot loader */
	MCUCR = 1 << IVCE;
	MCUCR = 1 << IVSEL;

	sei();
	wdt_reset();

	/* main bootloader code, skip if POR */
	if (mcusr & (_BV(WDRF) | _BV(EXTRF)) ||
			pgm_read_byte(zero) == 0xff)
		nrf_boot();

	leave_bootloader();

	for (;;);
}
