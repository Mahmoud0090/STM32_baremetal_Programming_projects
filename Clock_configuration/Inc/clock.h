#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void clock_config_hsi_16mhz(void);
void clock_config_hse_8mhz(void);
void clock_config_pll_84mhz(void);

// Optional helpers
uint32_t get_sysclk_freq(void);
uint32_t get_apb1_freq(void);
uint32_t get_apb2_freq(void);

#endif

