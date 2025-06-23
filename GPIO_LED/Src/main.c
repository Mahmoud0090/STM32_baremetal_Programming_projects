#include "stm32f4xx.h"


int main(void)
{

	RCC->AHB1ENR |= (1U<<0);


	GPIOA->MODER &= ~(3U << (4 * 2));   // Clear both bits for MODER4
	GPIOA->MODER |=  (1U << (4 * 2));   // Set MODER4 = 01 (output)


	while(1)
	{
		GPIOA->ODR ^= (1U<<4);

		for(int i = 0 ; i<100000 ; i++)
		{}
	}

	return 0;
}
