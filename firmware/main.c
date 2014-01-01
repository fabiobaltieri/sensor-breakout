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
#include <util/delay.h>

#include "board.h"

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

int __attribute__((noreturn)) main(void)
{
	led_init();

	wdt_enable(WDTO_1S);

	hello();
	led_off();

	sei();
	for (;;) {
		wdt_reset();
	}
}
