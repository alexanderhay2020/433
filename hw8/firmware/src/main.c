// Alexander Hay
// ME 433 - Adv. Mechatronics
// HW8

#include<xc.h>                      // processor SFR definitions
#include<sys/attribs.h>             // __ISR macro
#include<string.h>
#include<stdio.h>
#include<stdbool.h>

#include "i2c_master_noint.h"
#include "ssd1306.h"
#include "font.h"
#include "rtcc.h"

// DEVCFG0
#pragma config DEBUG = ON           // disable debugging
#pragma config JTAGEN = OFF         // disable jtag
#pragma config ICESEL = ICS_PGx1    // use PGED1 and PGEC1
#pragma config PWP = OFF            // disable flash write protect
#pragma config BWP = OFF            // disable boot write protect
#pragma config CP = OFF             // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL       // use primary oscillator with pll
#pragma config FSOSCEN = ON         // enable secondary oscillator
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

void drawBox(void){
    int i = 0;
    int j = 0;
    
    // upper left corner box placement
    int x = 72;
    int y = 22;
    
    
    // horizontal line
    for(i=y;i<32;i++){
        ssd1306_drawPixel(x,i,1);
    }
    
    // vertical line
    for(j=x;j<128;j++){
        ssd1306_drawPixel(j,y,1);
    }
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
    
    // variable declarations
    char FPS[20];
    char time_msg[50];
    char count_msg[5];
    char day_msg[11];

    unsigned long time = 0x21000000;      // 9pm (21:00:00)
    unsigned long date = 0x20052703;      // Wednesday, 5/27/2020
    
    bool a = true;
    int b = 1;
    
    int count = 0;
   
    // initializations
    initI2C();
    ssd1306_setup();
    rtcc_setup(time, date);
    
    // enable them interrupts
    __builtin_enable_interrupts();              

    while(1) {
        // characters are 5x8
        // screen is 128x32
        ssd1306_clear();
        
        // Heartbeat
        _CP0_SET_COUNT(0);      // Setting Core Timer count to 0
//        LATAbits.LATA4 = !LATAbits.LATA4;
        a = !a;
        
        // LCD heartbeat
        if (a==true){
            b = 1;
        }
        else{
            b = 0;
        }
        ssd1306_drawPixel(127,31,b); // flashes single LED on screen
        ssd1306_update();

        
        // main stuff
        
        // counter
        sprintf(count_msg, "cnt: %d", count);
        drawString(0,0,count_msg);
        
        // ticker
        rtccTime time = readRTCC();
        dayOfTheWeek(time.wk, day_msg);
        
        sprintf(time_msg, "date: %d%d/%d%d/%d%d", time.mn10, time.mn01, time.dy10, time.dy01, time.yr10, time.yr01);
        drawString(0,8,time_msg);
        sprintf(time_msg, "time: %d%d:%d%d:%d%d", time.hr10, time.hr01, time.min10, time.min01, time.sec10, time.sec01);
        drawString(0,16,time_msg);
        dayOfTheWeek(time.wk, day_msg);
        drawString(0,24,day_msg);
        
        // FPS stuff
//        drawBox();
        sprintf(FPS, "FPS: %3.1f", (500000.)/_CP0_GET_COUNT());
        drawString(74,0,FPS);

        ssd1306_update();
//        while(_CP0_GET_COUNT() < 24000000){;}    // 1Hz pulse
        while(_CP0_GET_COUNT() < 12000000){;}    // 2Hz pulse  
//        while(_CP0_GET_COUNT() < 4800000){;}    // 5Hz pulse
        count ++;
    }
}

