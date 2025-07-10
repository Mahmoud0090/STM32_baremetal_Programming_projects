#include "stm32f4xx.h"
#include "clock.h"

uint32_t sysclk, APB1 , APB2;

int main(void) {
    clock_config_pll_84mhz(); // Or HSI/HSE version
    sysclk = get_sysclk_freq();
    APB1 = get_apb1_freq();
    APB2 = get_apb2_freq();


    clock_config_hse_8mhz();
    sysclk = get_sysclk_freq();
    APB1 = get_apb1_freq();
    APB2 = get_apb2_freq();

    clock_config_hsi_16mhz();
    sysclk = get_sysclk_freq();
	APB1 = get_apb1_freq();
	APB2 = get_apb2_freq();

    while (1) {
        // ...
    }
}
