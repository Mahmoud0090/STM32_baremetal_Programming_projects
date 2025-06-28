#include "utils.h"

uint8_t bcd_to_decimal(uint8_t bcd)
{
	return (((bcd >> 4) * 10) + (bcd & 0x0F));
}

uint8_t decimal_to_bcd(uint8_t decimal)
{
	return (((decimal/10) << 4) | (decimal % 10));
}


