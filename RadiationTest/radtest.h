#ifndef RADTEST_h
#define RADTEST_h

#include <CCFlash.h>
#include <Energia.h>

#define TESTMEM SEGMENT_B
#define TESTMEMLEN INFO_SIZE

void erase_memory();
unsigned int check_memory();

#endif // RADTEST_h
