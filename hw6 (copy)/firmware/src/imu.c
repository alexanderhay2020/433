#include "imu.h"
#include "../hw6.X/i2c_master_noint.h"

void imu_setup(){
//    unsigned char who = 0;

    i2c_master_setup();

    // read from IMU_WHOAMI
    
//    if (who != 0b01101001){
//        while(1){}
//    }

    // init IMU_CTRL1_XL
    i2c_master_start();
    i2c_master_send(SLAVE | 0);          // set the address w/ write
    i2c_master_send(IMU_CTRL1_XL);          // location of CTRL_XL register
    i2c_master_send(0x82);          // set CTRL_XL register, 1.66kHz/2g/100Hz
    i2c_master_stop();              // send a stop
    
    // init IMU_CTRL2_G
    i2c_master_start();
    i2c_master_send(SLAVE | 0);          // set the address w/ write
    i2c_master_send(IMU_CTRL2_G);          // location of CTRL2_G register
    i2c_master_send(0x88);          // 1.66kHz/1000DPS
    i2c_master_stop();              // send a stop
    
    // init IMU_CTRL3_C
    i2c_master_start();
    i2c_master_send(SLAVE | 0);          // set the address w/ write
    i2c_master_send(IMU_CTRL3_C);          // location of CTRL3_C register
    i2c_master_send(0x04);          // set IF_INC to 1 to read from multiple registers
    i2c_master_stop();              // send a stop
}

void imu_read(unsigned char reg, signed short * data, int len){
    
    // read multiple from the imu, each data takes 2 reads so you need len*2 chars
    
    // turn the chars into the shorts
    
}