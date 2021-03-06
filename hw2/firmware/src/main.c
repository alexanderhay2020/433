// Alexander Hay
// ME 433 - Adv. Mechatronics
// HW2

#include<xc.h>                      // processor SFR definitions
#include<sys/attribs.h>             // __ISR macro
#include<math.h>
#include "../hw2.X/spi.h"

#define PI acos(-1)

// DEVCFG0
#pragma config DEBUG = ON           // disable debugging
#pragma config JTAGEN = OFF         // disable jtag
#pragma config ICESEL = ICS_PGx1    // use PGED1 and PGEC1
#pragma config PWP = OFF            // disable flash write protect
#pragma config BWP = OFF            // disable boot write protect
#pragma config CP = OFF             // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL       // use primary oscillator with pll
#pragma config FSOSCEN = OFF        // disable secondary oscillator
#pragma config IESO = OFF           // disable switching clocks
#pragma config POSCMOD = HS         // high speed crystal mode
#pragma config OSCIOFNC = OFF       // disable clock output
#pragma config FPBDIV = DIV_1       // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD       // disable clock switch and FSCM
#pragma config WDTPS = PS1048576    // use largest wdt
#pragma config WINDIS = OFF         // use non-window mode wdt
#pragma config FWDTEN = OFF         // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2     // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24     // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2     // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0           // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF       // allow multiple reconfigurations
#pragma config IOL1WAY = OFF        // allow multiple reconfigurations


void setVoltage(char a, unsigned short v) {
    
    unsigned short r;               // command register
    unsigned char b = 1;            // buffer: 1 for buffered, 0 for unbuffered
    unsigned char g = 1;            // gain: 1 for 1x, 0 for 2x
    unsigned char s = 1;            // shutdown: 1 for active, 0 for DAC
   
    r = (a<<15);
    r = r|(b<<14);
    r = r|(g<<13);
    r = r|(s<<12);
	r = r|((v&0b1111111111)<<2); //rejecting excessive bits (above 10)
	
	CS = 0;
	spi_io(r>>8);
	spi_io(r);
    CS = 1; 
}


int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;           // sets A4 as output
    TRISBbits.TRISB4 = 1;           // sets B4 as input
    LATAbits.LATA4 = 0;             // sets A4 to low

    __builtin_enable_interrupts();
    
    initSPI();
   
    // tune
    int a = 511;                // vertical shift
    int b = 511;                // amplitude
    int c = 100;                // period (1/f))
    
    int i = 0;
    int x = 0;
    float slope = 0;
    
    while(1) {
        _CP0_SET_COUNT(0);      // Setting Core Timer count to 0
        
        float tri = slope*x;    // y = mx+b
        x++;
        setVoltage(1, tri);
        if(x<=100) {
            slope = 10.23;
        }
        else if(x<=200) {
            slope = -10.23;
        }
        else {
            x = 0;
        }
        
        float sine = a + b * sin((2*PI*i)/c);   // y = a sin(b(x + c)) + d
        i++;
        setVoltage(0, sine);
        
        while(_CP0_GET_COUNT() < 24000) {}  // wait 1000 times a second
    }
}

