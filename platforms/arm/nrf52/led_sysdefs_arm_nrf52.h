#ifndef __LED_SYSDEFS_ARM_NRF52
#define __LED_SYSDEFS_ARM_NRF52

#ifndef NRF52
#define NRF52
#endif

#define LED_TIMER NRF_TIMER1	//NRF_TIMER1 used by Arduino BSP.
//#define FASTLED_NO_PINMAP
//#define FASTLED_HAS_CLOCKLESS

//#define FASTLED_SPI_BYTE_ONLY

#define FASTLED_ARM
//#define FASTLED_ARM_M0

#ifndef F_CPU
#define F_CPU 64000000
#endif

#include <stdint.h>
#include <nrf52.h>
//#include <core_cm0.h>

typedef volatile uint32_t RoReg;
typedef volatile uint32_t RwReg;
typedef uint32_t prog_uint32_t;
//typedef uint8_t boolean;

#define PROGMEM
#define NO_PROGMEM
#define NEED_CXX_BITS

// Default to NOT using PROGMEM here
#ifndef FASTLED_USE_PROGMEM
#define FASTLED_USE_PROGMEM 0
#endif

#ifndef FASTLED_ALLOW_INTERRUPTS
#define FASTLED_ALLOW_INTERRUPTS 1
#endif

#if FASTLED_ALLOW_INTERRUPTS == 1
#define FASTLED_ACCURATE_CLOCK
#endif

#define cli()  __disable_irq();
#define sei() __enable_irq();

#endif
