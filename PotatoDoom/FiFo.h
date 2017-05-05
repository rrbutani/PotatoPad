// FiFo.h
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Student names: Steven Zhu
// May 2, 2017


#include <stdint.h>

// *********** FIFO_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void FiFo_Init(void);

// *********** FIFO_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t FiFo_Put(char data);

// *********** FIFO_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t FiFo_Get(char *datapt);
