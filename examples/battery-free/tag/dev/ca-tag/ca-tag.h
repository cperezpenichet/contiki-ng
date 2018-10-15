/*
 * Copyright (c) 2018, Uppsala University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Carrier-assisted tag driver header file
 * \author
 *         Carlos Perez Penichet
 */

#ifndef CA_TAG_H_
#define CA_TAG_H_

#include "contiki.h"
#include "dev/radio.h"
#include "cc2420_const.h"

int ca_tag_init(void);

#define CA_TAG_MAX_PACKET_LEN      127

int ca_tag_set_channel(int channel);
int ca_tag_get_channel(void);

void ca_tag_set_pan_addr(unsigned pan,
			 unsigned addr,
			 const uint8_t *ieee_addr);

extern signed char ca_tag_last_rssi;
extern uint8_t ca_tag_last_correlation;

int ca_tag_rssi(void);

extern const struct radio_driver ca_tag_driver;

void ca_tag_set_txpower(uint8_t power);
int ca_tag_get_txpower(void);

/**
 * Interrupt function, called from the simple-ca_tag-arch driver.
 *
 */
int ca_tag_interrupt(void);

/* XXX hack: these will be made as Chameleon packet attributes */
extern rtimer_clock_t ca_tag_time_of_arrival,
  ca_tag_time_of_departure;
extern int ca_tag_authority_level_of_sender;

int ca_tag_on(void);
int ca_tag_off(void);

void ca_tag_set_cca_threshold(int value);

//extern const struct aes_128_driver cc2420_aes_128_driver;

#endif /* CA_TAG_H_ */
