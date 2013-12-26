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
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>

#include "board.h"
#include "nrf_boot.h"
#include "nrf24l01p.h"
#include "nrf_frames.h"
#include "flash.h"

static uint8_t EEMEM ee_board_id = 0x31;
static uint8_t board_id;

static struct nrf_frame frm;

#define BOOT_LOOP_LOAD (F_CPU / 250)
#define BOOT_TRY 5

static uint8_t boot_msg_is(struct nrf_frame *frm, uint8_t cmd)
{
	struct nrf_boot *bmsg = &frm->msg.boot;

	if (frm->board_id == board_id &&
	    frm->msg_id == NRF_MSG_ID_BOOT &&
	    frm->len == sizeof(*frm) &&
	    (bmsg->cmd & NRF_BOOT_CMD_MASK) == cmd)
		return 1;
	else
		return 0;
}

static void send_ack(void)
{
	struct nrf_boot *bmsg = &frm.msg.boot;

	_delay_ms(1);

	bmsg->cmd = NRF_BOOT_CMD_ACK;

	nrf_standby();
	nrf_tx((uint8_t *)&frm, sizeof(frm));
}

/* send CMD_BOOT, wait for CMD_START */
static uint8_t send_beacons(void)
{
	volatile int zero = 0;
	struct nrf_boot *bmsg = &frm.msg.boot;
	uint8_t i;
	uint16_t wait_loop;

	for (i = 0; i < BOOT_TRY || pgm_read_byte(zero) == 0xff; i++) {
		wdt_reset();

		/* send boot frame */
		memset(&frm, 0, sizeof(frm));
		frm.board_id = board_id;
		frm.msg_id = NRF_MSG_ID_BOOT;
		frm.len = sizeof(frm);
		frm.seq = i;
		bmsg->cmd = NRF_BOOT_CMD_BOOT;

		nrf_standby();
		nrf_tx((uint8_t *)&frm, sizeof(frm));

		/* wait for start frame or bail out */
		for (wait_loop = 0; wait_loop < BOOT_LOOP_LOAD; wait_loop++) {
			if (!nrf_poll())
				continue;

			nrf_rx((uint8_t *)&frm, sizeof(frm));
			if (boot_msg_is(&frm, NRF_BOOT_CMD_START)) {
				send_ack();
				/* continue */
				return 0;
			}
		}

		led_toggle();
	}

	/* timeout */
	return 1;
}

static uint8_t do_write(struct nrf_frame *frm)
{
	struct nrf_boot *bmsg = &frm->msg.boot;

	static uint8_t last_good = 0xff;
	uint8_t size;
	uint8_t sum;
	uint8_t i;

	size = NRF_BOOT_SIZE(bmsg->cmd);

	/* check size limit */
	if (size > 10)
		return 1;

	/* validate checksum */
	sum = 0;
	for (i = 0; i < size; i++)
		sum += bmsg->payload[i];

	if (bmsg->cksum != sum)
		return 1;

	/* validate sequence and discard duplicates (but still ack) */
	if (frm->seq == last_good)
		return 0;

	if (frm->seq != ((last_good + 1) & 0xff))
		return 1;

	last_good = frm->seq;

	/* write out */
	flash_write(bmsg->payload, size);

	return 0;
}

/* wait for CMD_WRITE or CMD_FINALIZE */
static uint8_t wait_write(void)
{
	wdt_reset();

	for (;;) {
		if (!nrf_poll())
			continue;

		nrf_rx((uint8_t *)&frm, sizeof(frm));
		if (boot_msg_is(&frm, NRF_BOOT_CMD_WRITE(0))) {
			if (!do_write(&frm))
				send_ack();

			/* wait for more */
			return 1;
		} else if (boot_msg_is(&frm, NRF_BOOT_CMD_FINALIZE)) {
			flash_end_write();
			send_ack();

			/* leave bootloader */
			return 0;
		}
	}
}

void nrf_boot(void)
{
	uint8_t ret;

	board_id = eeprom_read_byte(&ee_board_id);

	ret = send_beacons();
	if (ret)
		return;

	flash_start();

	while (wait_write())
		led_toggle();

	_delay_ms(200);

	nrf_powerdown();
}
