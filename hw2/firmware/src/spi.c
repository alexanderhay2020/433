// initialize SPI1

#include<xc.h>                      // processor SFR definitions
#include<sys/attribs.h>             // __ISR macro
#include"../hw2.X/spi.h"

void initSPI() {
    // Pin B14 has to be SCK1
    // done
    
    // Turn off analog pins
    //...
    ANSELA = 0;
    ANSELB = 0;

    // Make an output pin for CS
    //...8
    //...
    TRISAbits.TRISA0 = 0;           // sets A0 as output
    CS = 1;                         // sets A0 to high

    // Set SDO1
    //... set to a1, because A1 (pin3) is wired to SDI (pin5) on the MCP
    RPA1Rbits.RPA1R = 0b0011;
   
    // Set SDI1
    //...set to b5, because why?
    SDI1Rbits.SDI1R = 0b0001;

    
    // setup SPI1
    SPI1CON = 0;                    // turn off the spi module and reset it
    SPI1BUF;                        // clear the rx buffer by reading from it
    SPI1BRG = 1000;                 // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;        // clear the overflow bit
    SPI1CONbits.CKE = 1;            // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;          // master operation
    SPI1CONbits.ON = 1;             // turn on spi 
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}