#include "stdafx.h";

unsigned char xor_sum(unsigned char *buffer, unsigned int length)
{
	unsigned char temp_sum = 0;

	while (length-- > 0)
	{
		temp_sum ^= *buffer++;
	}
	return (temp_sum);
}
