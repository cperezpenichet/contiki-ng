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
 *         Carrier-assisted tag driver 
 * \author
 *         Carlos Perez Penichet
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "contiki.h"
#include "sys/energest.h"

#include "dev/uart0.h"

#if defined(__AVR__)
#include <avr/io.h>
#endif

#include "dev/leds.h"
//#include "dev/spi.h"
#include "ca-tag.h"
#include "cc2420_const.h"

#include "net/packetbuf.h"
#include "net/netstack.h"

#define CHECKSUM_LEN        2
#define FOOTER_LEN          2

enum write_ram_order {
  /* Begin with writing the first given byte */
  WRITE_RAM_IN_ORDER,
  /* Begin with writing the last given byte */
  WRITE_RAM_REVERSE
};

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif

#define DEBUG_LEDS DEBUG
#undef LEDS_ON
#undef LEDS_OFF
#if DEBUG_LEDS
#define LEDS_ON(x) leds_on(x)
#define LEDS_OFF(x) leds_off(x)
#else
#define LEDS_ON(x)
#define LEDS_OFF(x)
#endif

void cc2420_arch_init(void);

/* XXX hack: these will be made as Chameleon packet attributes */
rtimer_clock_t ca_tag_time_of_arrival, ca_tag_time_of_departure;

int ca_tag_authority_level_of_sender;

volatile uint8_t ca_tag_sfd_counter;
volatile uint16_t ca_tag_sfd_start_time;
volatile uint16_t ca_tag_sfd_end_time;

static volatile uint16_t last_packet_timestamp;
/*---------------------------------------------------------------------------*/
PROCESS(ca_tag_process, "ca-tag driver");
/*---------------------------------------------------------------------------*/

int ca_tag_on(void);
int ca_tag_off(void);

static int ca_tag_read(void *buf, unsigned short bufsize);

static int ca_tag_prepare(const void *data, unsigned short len);
static int ca_tag_transmit(unsigned short len);
static int ca_tag_send(const void *data, unsigned short len);

static int ca_tag_receiving_packet(void);
static int pending_packet(void);
static int ca_tag_cca(void);

static uint8_t receive_on;
static uint8_t transmit_on;
static uint8_t packetbuf_cursor;
static uint8_t expect_high_next;

static uint8_t payload_len_duration;

static void uart0_write_line(char *line)  {
    int cursor= 0;
    
    while(line[cursor] != 0) {
        uart0_writeb(line[cursor]);
        //SPI_WRITE(line[cursor]);
        cursor++;
    }   
}

static radio_result_t
get_value(radio_param_t param, radio_value_t *value)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}

static radio_result_t
set_value(radio_param_t param, radio_value_t value)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}

static radio_result_t
get_object(radio_param_t param, void *dest, size_t size)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}

static radio_result_t
set_object(radio_param_t param, const void *src, size_t size)
{
  return RADIO_RESULT_NOT_SUPPORTED;
}

const struct radio_driver ca_tag_driver =
  {
    ca_tag_init,
    ca_tag_prepare,
    ca_tag_transmit,
    ca_tag_send,
    ca_tag_read,
    ca_tag_cca,
    ca_tag_receiving_packet,
    pending_packet,
    ca_tag_on,
    ca_tag_off,
    get_value,
    set_value,
    get_object,
    set_object
  };

/*---------------------------------------------------------------------------*/
static void
write_fifo_buf(const uint8_t *buffer, uint16_t count)
{
  uint8_t i;
  char temp[3];
  temp[2] = 0;
  
  for(i = 0; i < count; i++) {
    sprintf(temp, "%x%x", buffer[i] >> 4, buffer[i] & 0x0F);
    uart0_write_line(temp);
  }
}
/*---------------------------------------------------------------------------*/
static void
wait_for_transmission(void)
{
  rtimer_clock_t t0;
  t0 = RTIMER_NOW();
  uint32_t duration = payload_len_duration + payload_len_duration/10 + 5;
  PRINTF("ca_tag: waiting for TX: %d\n", transmit_on);
  while(transmit_on 
      && RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + duration));
      //&& RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (RTIMER_SECOND / 2000)));

  if (transmit_on)
      PRINTF("ca_tag: timeout!\n");

}
/*---------------------------------------------------------------------------*/
void
reset_packetbuf() {
    packetbuf_clear();
    packetbuf_cursor = 0;
}
/*---------------------------------------------------------------------------*/
static void
on(void)
{
  printf("ca_tag: actual on\n");

  uart0_write_line("r\n");

  ENERGEST_ON(ENERGEST_TYPE_LISTEN);
  receive_on = 1;
  expect_high_next = 1;
  //reset_packetbuf();
}
/*---------------------------------------------------------------------------*/
static void
off(void)
{
  receive_on = 0;

  /* Wait for transmission to end before turning radio off. */
  wait_for_transmission();

  ENERGEST_OFF(ENERGEST_TYPE_LISTEN);
  uart0_write_line("o\n");
}
/*---------------------------------------------------------------------------*/
static uint8_t locked, lock_on, lock_off;
#define GET_LOCK() locked++
static void RELEASE_LOCK(void) {
  if(locked == 1) {
    if(lock_on) {
      on();
      lock_on = 0;
    }
    if(lock_off) {
      off();
      lock_off = 0;
    }
  }
  locked--;
}
/*---------------------------------------------------------------------------*/
int 
uart_byte_rx_callback(unsigned char c) {
    char *buff;
    char str[2];
    str[1] = 0;
    uint8_t value;
    
    if (transmit_on && c == 'k') {  // Just finished transmitting
        PRINTF("ca_tag: callback tx off\n");
        transmit_on = 0;
        return 1;
    } else if (receive_on) {
        PRINTF("ca_tag: Serial Callback: %c \n", c);
        if (c == '\n') {
            expect_high_next = 1;
            process_poll(&ca_tag_process);
        } else if (c == 'k') {
        } else {
            buff = packetbuf_dataptr();
            str[0] = c;
            value = (uint8_t)strtol(str, NULL, 16);
            if (expect_high_next) {
                buff[packetbuf_cursor] = value<<4;
                expect_high_next = 0;
            } else {
                buff[packetbuf_cursor] |= value;
                expect_high_next = 1;
                packetbuf_cursor++;
            }
        } 
    }
    return 1;
}
/*---------------------------------------------------------------------------*/
int
ca_tag_init(void)
{
  PRINTF("ca_tag: ca_tag_driver_init\n");
  //ca_tag_off(); // TODO: Turn off the real cc2420?

  cc2420_arch_init();

  /*spi_init();
  CC2420_DISABLE_FIFOP_INT();
  CC2420_SPI_ENABLE();
  SPI_WRITE("o\n");*/
  uart0_set_input(uart_byte_rx_callback);
  uart0_init(UART0_BAUD2UBR(115200ul));

  uart0_write_line("o\n");
//  CC2420_SPI_DISABLE();
  receive_on = 0;
  transmit_on = 0;

  process_start(&ca_tag_process, NULL);
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
ca_tag_transmit(unsigned short payload_len)
{
  int i;
  payload_len_duration = payload_len;
  
  GET_LOCK();

  printf("ca_tag: Transmit now\n");
  //while(i < 30000) i++;

  /* The TX FIFO can only hold one packet. Make sure to not overrun
   * FIFO by waiting for transmission to start here and synchronizing
   * with the CC2420_TX_ACTIVE check in cc2420_send.
   *
   * Note that we may have to wait up to 320 us (20 symbols) before
   * transmission starts.
   */
#ifndef CC2420_CONF_SYMBOL_LOOP_COUNT
#error CC2420_CONF_SYMBOL_LOOP_COUNT needs to be set!!!
#else
#define LOOP_20_SYMBOLS CC2420_CONF_SYMBOL_LOOP_COUNT
#endif

  // Initiate transmission
  uart0_write_line("\n");
  transmit_on = 1;

  for(i = LOOP_20_SYMBOLS; i > 0; i--) {

      if(receive_on) {
	ENERGEST_OFF(ENERGEST_TYPE_LISTEN);
      }
      ENERGEST_ON(ENERGEST_TYPE_TRANSMIT);
      /* We wait until transmission has ended so that we get an
	 accurate measurement of the transmission time.*/
      wait_for_transmission();

//#ifdef ENERGEST_CONF_LEVELDEVICE_LEVELS
//      ENERGEST_OFF_LEVEL(ENERGEST_TYPE_TRANSMIT,ca_tag_get_txpower());
//#endif
      ENERGEST_OFF(ENERGEST_TYPE_TRANSMIT);
      if(receive_on) {
        on();
      } else {
	/* We need to explicitly turn off the radio,
	 * since STXON[CCA] -> TX_ACTIVE -> RX_ACTIVE */
        off();
      }

      RELEASE_LOCK();
      return RADIO_TX_OK;
  }

  /* If we send with cca (cca_on_send), we get here if the packet wasn't
     transmitted because of other channel activity. */
  //PRINTF("cc2420: do_send() transmission never started\n");

  RELEASE_LOCK();
  return RADIO_TX_COLLISION;
}
/*---------------------------------------------------------------------------*/
static int
ca_tag_prepare(const void *payload, unsigned short payload_len)
{
  GET_LOCK();

  PRINTF("ca_tag: sending %d bytes\n", payload_len);

  // Set the tag in TX mode.
  uart0_write_line("s\n");

  /* Write packet to TX FIFO. */
  write_fifo_buf(payload, payload_len);
  
  RELEASE_LOCK();
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
ca_tag_send(const void *payload, unsigned short payload_len)
{
  ca_tag_prepare(payload, payload_len);
  return ca_tag_transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
int
ca_tag_off(void)
{
  /* Don't do anything if we are already turned off. */
  if(receive_on == 0) {
    return 1;
  }

  /* If we are called when the driver is locked, we indicate that the
     radio should be turned off when the lock is unlocked. */
  if(locked) {
    /*    printf("Off when locked (%d)\n", locked);*/
    lock_off = 1;
    return 1;
  }

  GET_LOCK();
  off();
  RELEASE_LOCK();
  return 1;
}
/*---------------------------------------------------------------------------*/
int
ca_tag_on(void)
{
  if(receive_on) {
    return 1;
  }
  if(locked) {
    lock_on = 1;
    return 1;
  }

  GET_LOCK();
  on();
  RELEASE_LOCK();
  return 1;
}
/*---------------------------------------------------------------------------*/
int
ca_tag_interrupt(void)
{
  //CC2420_CLEAR_FIFOP_INT();
  //process_poll(&ca_tag_process);

  last_packet_timestamp = ca_tag_sfd_start_time;
  return 1;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ca_tag_process, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("ca_tag_process: started\n");

  while(1) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

    PRINTF("ca_tag_process: calling receiver callback\n");

    //packetbuf_clear();???
    packetbuf_set_attr(PACKETBUF_ATTR_TIMESTAMP, ca_tag_sfd_start_time);
//    len = ca_tag_read(packetbuf_dataptr(), PACKETBUF_SIZE);
//    
    packetbuf_set_datalen(packetbuf_cursor);
    packetbuf_cursor = 0;

    NETSTACK_MAC.input();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static int
ca_tag_read(void *buf, unsigned short bufsize)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
ca_tag_cca(void)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
int
ca_tag_receiving_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
pending_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
