// SD.c
// Runs on TM4C123
// SD module to load data
// January 13, 2015
// Steven Zhu
// Last Modified: 4/8/2017

#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "diskio.h"
#include "ff.h"

#include "driver/ST7735.h"
#include "driver/Print.h"

typedef struct {
	uint32_t offset;
	uint32_t size;
	char	name[8];
} Lump;

static FATFS FatFs;
FIL file;
char buffer[512];	// Max buffer size is 512 bytes
FRESULT result;

// Read with error handling
void read(uint32_t bufferSize) {
	uint32_t check;
	uint32_t temp = f_tell(&file);	//copy old data
	while ((f_read(&file, &buffer, bufferSize, &check) != FR_OK) ||
		(check != bufferSize)) {
		f_tell(&file) = temp;
	};
}
	
void loadData(void) {
	ST7735_InitR(INITR_REDTAB);
	uint32_t lumpNum;
	result = f_mount(&FatFs, "", 0);
  while(result){
		ST7735_OutString("Mount error"); 
	}	// Error handler
  // open the file to be read
	result = f_open(&file, "DOOM.WAD", FA_READ);	// FIXME

	if(result == FR_OK){
			ST7735_DrawString(0, 14, "File opened ", ST7735_Color565(255, 0, 0));
			while (1) {}
//		f_tell(&file)++;
//		read(&file, &buffer, 4);
//		lumpNum = (uint32_t)buffer;
//		Lump lumpList[lumpNum];
//    read(&file, &buffer, 4);
//		f_tell(&file) = (uint32_t)buffer;	// Set ptr to directory
//		for (int i = 0; i < lumpNum; i++) {
//			read(&file, &buffer, 16);
//			lumpList[i] = *(Lump*)buffer;
//			ST7735_SetCursor(0,i * 6);
//			ST7735_OutString(lumpList[i].name); 
//		}
//		while(1){};
//    // close the file
//    f_close(&file);
  }
	ST7735_DrawString(0, 14, "Error ", ST7735_Color565(255, 0, 0));
	ST7735_SetCursor(16, 0);
	ST7735_OutUDec((uint32_t)result);

	while (1) {
		
	}
}
