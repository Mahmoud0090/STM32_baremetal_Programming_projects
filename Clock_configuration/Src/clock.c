#include "stm32f4xx.h"
#include "clock.h"

//private helper: wait for bit

static void wait_until_set(volatile uint32_t reg , uint32_t mask)
{
	while(!(reg & mask));
}

//1. Using HSI directly
void clock_config_hsi_16mhz(void)
{
	RCC->CR |= (1U<<0); // enabling HSI bit
	wait_until_set(RCC->CR, (1U<<1)); //HSI ready bit

	RCC->CFGR = 0; // Reset CFGR (uses HSI, default prescalers)
}

//2. Use HSE directly (assuming 8 MHz crystal)
void clock_config_hse_8mhz(void)
{
	RCC->CR |= (1U<<16); //enabling HSE bit
	wait_until_set(RCC->CR , (1U<<17)); //HSE ready bit

	RCC->CFGR &= ~(3U<<0); // clear SW[1:0]: System clock switch
	RCC->CFGR |= (1U<<0); // 01 for HSE selection

	while((RCC->CFGR & (3U<<2)) != (1U<<2));
}

//3. Use PLL to get 84 MHz SYSCLK
// HSE (8 MHz) → PLLM = 8, PLLN = 168, PLLP = 2 → 84 MHz
void clock_config_pll_84mhz(void)
{
	RCC->CR |= (1U<<16); //enabling HSE bit
	wait_until_set(RCC->CR , (1U<<17)); //HSE ready bit


	// 2. Set FLASH wait states (2 WS for 84 MHz, voltage scale 2)
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |
				 FLASH_ACR_DCEN | FLASH_ACR_LATENCY_2WS;

	// 3. Configure PLL
	// PLLM = 8, PLLN = 168, PLLP = 2 (00), PLLSRC = HSE, PLLQ = 4
	RCC->PLLCFGR = (8  << 0)   |  // PLLM [5:0]
	               (168 << 6)  |  // PLLN [14:6]
	               (0  << 16)  |  // PLLP [17:16] => 00 means ÷2
	               (1  << 22)  |  // PLLSRC: 0 = HSI, 1 = HSE
	               (4  << 24);    // PLLQ [27:24]

	// 4. Enable PLL
	RCC->CR |= (1 << 24); // PLLON
	while (!(RCC->CR & (1 << 25))); // Wait for PLLRDY

	// 5. Set prescalers
	RCC->CFGR &= ~((0xF << 4) | (0x7 << 10) | (0x7 << 13)); // Clear HPRE[7:4], PPRE1[12:10], PPRE2[15:13]
	RCC->CFGR |= (0 << 4);    // AHB prescaler = /1 (HPRE)
	RCC->CFGR |= (5 << 10);   // APB1 prescaler = /4 (PPRE1: 101)
	RCC->CFGR |= (4 << 13);   // APB2 prescaler = /2 (PPRE2: 100)

	// 6. Switch system clock to PLL
	RCC->CFGR &= ~(0x3 << 0); // Clear SW bits
	RCC->CFGR |=  (0x2 << 0); // Set SW = 10 (PLL)
	while (((RCC->CFGR >> 2) & 0x3) != 0x2); // Wait until SWS == 10 (PLL selected)
}

uint32_t get_sysclk_freq(void) {
    switch ((RCC->CFGR >> 2) & 0x3) {
        case 0: return 16000000; // HSI
        case 1: return 8000000;  // HSE
        case 2: return 84000000; // PLL (assuming config)
        default: return 0;
    }
}

uint32_t get_apb1_freq(void) {
    uint32_t prescaler_bits = (RCC->CFGR >> RCC_CFGR_PPRE1_Pos) & 0x7; //RCC_CFGR_PPRE1_Pos = 10
    uint32_t divisor;

    if (prescaler_bits < 0b100) divisor = 1;
    else if (prescaler_bits == 0b100) divisor = 2;
    else if (prescaler_bits == 0b101) divisor = 4;
    else if (prescaler_bits == 0b110) divisor = 8;
    else divisor = 16;

    return get_sysclk_freq() / divisor;
}

uint32_t get_apb2_freq(void) {
    uint32_t prescaler_bits = (RCC->CFGR >> RCC_CFGR_PPRE2_Pos) & 0x7;
    uint32_t divisor;

    if (prescaler_bits < 0b100) divisor = 1;
    else if (prescaler_bits == 0b100) divisor = 2;
    else if (prescaler_bits == 0b101) divisor = 4;
    else if (prescaler_bits == 0b110) divisor = 8;
    else divisor = 16;

    return get_sysclk_freq() / divisor;
}
