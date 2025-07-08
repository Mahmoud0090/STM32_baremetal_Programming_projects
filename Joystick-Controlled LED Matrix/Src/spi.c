#include "stm32f4xx.h"
#include "spi.h"

void spi1_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // PA5=SCK, PA7=MOSI, PA4=CS
    GPIOA->MODER &= ~((3U<<(5*2)) | (3U<<(7*2)) | (3U<<(4*2)));
    GPIOA->MODER |=  ((2U<<(5*2)) | (2U<<(7*2)) | (1U<<(4*2)));
    GPIOA->AFR[0] |= (5U<<(5*4)) | (5U<<(7*4));

    GPIOA->OSPEEDR |= (3U<<(5*2)) | (3U<<(7*2)) | (3U<<(4*2)); // High speed
    GPIOA->OTYPER &= ~(1U << 4); // Push-pull

    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | (2 << 3); // fPCLK/8
    SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_write(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    while (SPI1->SR & SPI_SR_BSY);
}


