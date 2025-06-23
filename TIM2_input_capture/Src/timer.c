#include "stm32f4xx.h"
#include "timer.h"


#define GPIOAEN 		(1U<<0)
#define TIM2EN	 		(1U<<0)
#define TIM2_SR_CC1IF	(1U<<1)

volatile uint32_t last_capture = 0;
volatile uint32_t frequency = 0;

void gpio_pa0_input_init(void)
{
	//enable GPIOA clock
	RCC->AHB1ENR |= GPIOAEN;

	// Set PA0 to alternate function
	GPIOA->MODER &= ~(3U<<(0 * 2));
	GPIOA->MODER |= (2U<<(0 * 2));

	// AF1 for TIM2_CH1 on PA0
	GPIOA->AFR[0] &= ~(0xF<<(0 * 4));
	GPIOA->AFR[0] |= (1U<<(0 * 4));
}

void tim2_input_capture_init(void)
{
	//enable TIM2 clock
	RCC->APB1ENR |= TIM2EN;

	//Prescaler: scales down clock to 1MHZ (assuming 16Mhz clock)
	TIM2->PSC = 16 - 1;

	//max count
	TIM2->ARR = 0xFFFFFFFF;

	//set channel 1 as input , mapped to TI1
	TIM2->CCMR1 &= ~(3U<<0);
	TIM2->CCMR1 |= (1U<<0);

	// Capture on rising edge
	TIM2->CCER &= ~(1U<<1); // CC1P = 0 → rising edge

	//enable capture on channel 1
	TIM2->CCER |= (1U<<0); // CC1E = 1

	//enable interrupt on capture compare 1
	TIM2->DIER |= (1U<<1); // CC1IE

	// Enable timer
	TIM2->CR1 |= (1U<<0);

	// Enable interrupt in NVIC
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
	if(TIM2->SR & TIM2_SR_CC1IF) // Check CC1IF
	{
		static uint32_t current_capture = 0;
		current_capture = TIM2->CCR1;

		uint32_t diff = current_capture - last_capture;
		last_capture = current_capture;

		// 1 tick = 1 us → frequency = 1,000,000 / diff
		if(diff != 0)
			frequency = 1000000/diff;

		TIM2->SR &= ~(1U<<1); // Clear CC1IF
	}
}
