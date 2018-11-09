#ifndef __PROJECT_CONF_H__
#define __PROJECT_CONF_H__

#define UART0_CONF_WITH_INTERRUPT 0

#undef UART0_CONF_RX_WITH_DMA
#define UART0_CONF_RX_WITH_DMA    1

#undef NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO ca_tag_driver

#define ENERGEST_CONF_ON 1

/* Disable DCO calibration (uses timerB) */
#undef DCOSYNCH_CONF_ENABLED
#define DCOSYNCH_CONF_ENABLED		0

/* Enable SFD timestamps (uses timerB) */
#undef CC2420_CONF_SFD_TIMESTAMPS
#define CC2420_CONF_SFD_TIMESTAMPS      1

//#define LOG_CONF_LEVEL_NULLNET LOG_LEVEL_DBG
//#define LOG_CONF_LEVEL_MAC LOG_LEVEL_DBG

#endif /* __PLATFORM_CONF_H__ */
