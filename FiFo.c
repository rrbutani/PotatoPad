// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Student names: Junlin Zhu
// Last modification date: 04/13/2017

#include <stdint.h>
// --UUU-- Declare state variables for FiFo
//        size, buffer, put and get indexes
#define size 8
static uint8_t putIndex;
static uint8_t getIndex;
static uint8_t buffer[size];
static uint8_t count;

// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void FiFo_Init() {
	getIndex = 0;
	putIndex = 0;
	count = 0;
}

// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t FiFo_Put(char data) {
	if (count == size)
		return 0;
	buffer[putIndex] = data;
	putIndex = (putIndex + 1) % size;
	count++;
	return 1;
}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t FiFo_Get(char *datapt) {
	if (count == 0)
		return 0;
	*datapt = buffer[getIndex];
	getIndex = (getIndex + 1) % size;
	count--;
	return 1;
}



