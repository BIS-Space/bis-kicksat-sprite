#include "radtest.h"

byte read_check(byte offset);
byte bitcount(byte mem);

void erase_memory()
{
	Flash.erase(TESTMEM);
}

unsigned int check_memory()
{
	int count = 0;

	for (byte i = 0; i < TESTMEMLEN; i++)
	{
		count += read_check(i);
	}

	if (count == TESTMEMLEN*8) erase_memory();

	return count;
}

byte read_check(byte offset)
{
	byte stored = 0;

	Flash.read(TESTMEM+offset,&stored,1);

	return bitcount(stored);
}

byte bitcount(byte mem)
{
	/* count how many 0s are in an 8 bit binary number */
	/* optimised for dense ones, as in newly erased memory */
	byte count = 0;

	while(mem - 0xFF)
	{
		count++;
		mem |= mem+1;
	}

	return count;
}
