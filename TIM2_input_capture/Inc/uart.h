/*
 * uart.h
 *
 *  Created on: Jun 15, 2025
 *      Author: User
 */

#ifndef UART_H_
#define UART_H_

void uart2_init(void);
void uart2_write(char c);
void uart2_print(char* s);
void uart2_print_uint32(uint32_t num);


#endif /* UART_H_ */
