// Alexander Hay
// ME 433 - Adv. Mechatronics
// HW3

#include<xc.h>                      // processor SFR definitions
#include<sys/attribs.h>             // __ISR macro
#include "../hw3.X/i2c_master_noint.h"

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


void initI2C(void);
void setPin(unsigned char add, unsigned char reg, unsigned char val);  // address, register, value
char readPin(unsigned char add,unsigned char reg);           // address, register


void initI2C(void){

    i2c_master_setup();
    i2c_master_start();
    i2c_master_send(0x40);        // set the address w/ write
    i2c_master_send(0x00);          // set the register (IODIRA)
    i2c_master_send(0x00);          // set the value    (0000 0000) for outputs
    i2c_master_stop();              // send a stop
    i2c_master_start();
    i2c_master_send(0x40);        // set the address w/ write
    i2c_master_send(0x01);          // set the register (IODIRB)
    i2c_master_send(0xFF);          // set the value    (1111 1111) for inputs
    i2c_master_stop();              // send a stop
}


void setPin(unsigned char add, unsigned char reg, unsigned char val){
    
    i2c_master_start();
    i2c_master_send(add);
    i2c_master_send(reg);
    i2c_master_send(val);          // FF-yellow on; 00-yellow off
    i2c_master_stop();
}


char readPin(unsigned char add, unsigned char reg){
    
    i2c_master_start();
    i2c_master_send(add|0);         // address for writing
    i2c_master_send(reg);           // register to read from
    i2c_master_restart();           // restart
    i2c_master_send(add|0x1);         // address for reading
    
    char recv = i2c_master_recv();  // received value
    
    i2c_master_ack(1);              // acknowledge bit
    i2c_master_stop();              // stop
    
    return recv;
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
    LATAbits.LATA4 = 1;             // sets A4 to low; 0-off, 1-on
    
    initI2C();
    
    __builtin_enable_interrupts();

    unsigned char slave_add = 0x40;         // (given) 0100 0000
    unsigned char button_pin = 0x13;        // GPIOB
    unsigned char led_pin = 0x14;           // OLATA
    unsigned char on = 0xFF;                // 1111 1111
    unsigned char off = 0x00;               // 0000 0000
    unsigned char button;                   

    while(1) {
        
        // Heartbeat
        _CP0_SET_COUNT(0);      // Setting Core Timer count to 0
        LATAbits.LATA4 = !LATAbits.LATA4;
        while(_CP0_GET_COUNT() < 4800000){;}    // 5Hz pulse
        
        button = readPin(slave_add, button_pin);      // read from slave, GPIOB
        
        while(!button){
            
            setPin(slave_add, led_pin, on);         // write to slave, OLATA, state
//            while(_CP0_GET_COUNT() < 4800000){;}    // 5Hz pulse

            break;
        }
        
        setPin(slave_add, led_pin, off);
    }
}
