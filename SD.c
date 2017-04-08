// SD.c
// Runs on TM4C123
// SD module to load data
// Steven Zhu
// Last Modified: 4/8/2017

#include "SD.h"
#include "tm4c123gh6pm.h"
#include "diskio.h"
#include "ff.h"

static FATFS g_sFatFs;
FIL Handle,Handle2;
FRESULT MountFresult;
FRESULT Fresult;
unsigned char buffer[512];
#define MAXBLOCKS 100

const char inFilename[] = "DOOM.WAD";   // 8 characters or fewer

void loadData(void) {
	UINT successfulreads, successfulwrites;
  uint8_t c, x, y;
  MountFresult = f_mount(&g_sFatFs, "", 0);
  if(MountFresult){
    while(1){};
  }
  // open the file to be read
  Fresult = f_open(&Handle, inFilename, FA_READ);
  if(Fresult == FR_OK){
    // get a character in 'c' and the number of successful reads in 'successfulreads'
    Fresult = f_read(&Handle, &c, 1, &successfulreads);
    x = 0;                              // start in the first column
    y = 10;                             // start in the second row
    while((Fresult == FR_OK) && (successfulreads == 1) && (y <= 130)){
      if(c == '\n'){
        x = 0;                          // go to the first column (this seems implied)
        y = y + 10;                     // go to the next row
      } else if(c == '\r'){
        x = 0;                          // go to the first column
      } else{                           // the character is printable, so print it
        x = x + 6;                      // go to the next column
        if(x > 122){                    // reached the right edge of the screen
          x = 0;                        // go to the first column
          y = y + 10;                   // go to the next row
        }
      }
      // get the next character in 'c'
      Fresult = f_read(&Handle, &c, 1, &successfulreads);
    }
    // close the file
    Fresult = f_close(&Handle);
  } else{
  }
}
