// initialize SPI1

#include<xc.h>                      // processor SFR definitions
#include<sys/attribs.h>             // __ISR macro

void initSPI() {
    // Pin B14 has to be SCK1
    // done
    
    // Turn off analog pins
    //...
    AD1CON1bits.ON = 0;                     // bit 15 sets it OFF

    
    // Make an output pin for CS
    //...
    //...
    
    
    // Set SDO1
    //...
    
    // Set SDI1
    //...

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1000; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}