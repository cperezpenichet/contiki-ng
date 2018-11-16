/*
 * Copyright (c) 2018, Uppsala University
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
 *         CC2420 carrier generator example
 * \author
 *         Carlos Perez Penichet
 */

#include "contiki.h"
#include "dev/cc2420/cc2420.h"
#include "dev/cc2420/cc2420_const.h"

#include <stdio.h>
#include "sys/log.h"
#define LOG_MODULE "Carrier Gen"
#define LOG_LEVEL LOG_LEVEL_INFO

/* --------------------------------------------- */
PROCESS(carrier_test, "Carrier_test");
AUTOSTART_PROCESSES(&carrier_test);
/* --------------------------------------------- */

PROCESS_THREAD(carrier_test, ev, data) {
    PROCESS_BEGIN();   
    
    static int channel;
    static int power;    
    
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 24);
    NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL, &channel);
    LOG_INFO("Channel: %d\n", channel);
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, 0);
    NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_TXPOWER, &power);
    LOG_INFO("Power: %d dBm\n", power);
     
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_POWER_MODE, RADIO_POWER_MODE_CARRIER_ON);

    PROCESS_END();
}
