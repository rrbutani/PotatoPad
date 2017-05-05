// ADC.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Steven Zhu
// 5/1/17

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: two 8-bit result of ADC conversion for x
void ADC_In(uint8_t data[2]);
