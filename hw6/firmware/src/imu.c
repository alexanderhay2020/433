#include "imu.h"
#include "../hw6.X/i2c_master_noint.h"

char imu_setup(){
  
    unsigned char who = 0;

    i2c_master_setup();

    // read from IMU_WHOAMI
    i2c_master_start();
    i2c_master_send(IMU_ADDR | 0);      // write the address, shifted left by 1, or'ed with a 0 to indicate writing
    i2c_master_send(IMU_WHOAMI);        // read from WHO_AM_I register addressed at 0x0F
    i2c_master_restart();               // make the restart bit
    i2c_master_send(IMU_ADDR  | 1);     // write the address, shifted left by 1, or'ed with a 1 to indicate reading
    
    who = i2c_master_recv();            // save the value returned
    
    i2c_master_ack(1);                  // make the ack bit
    i2c_master_stop();                  // make the stop bit
    
    if (who != 0b01101001){
        while(1){}
    }

    // init IMU_CTRL1_XL
    i2c_master_start();
    i2c_master_send(IMU_ADDR | 0);      // set the address w/ write
    i2c_master_send(IMU_CTRL1_XL);      // location of CTRL_XL register
    i2c_master_send(0x82);              // set CTRL_XL register, 1.66kHz/2g/100Hz
    i2c_master_stop();                  // send a stop
    
    // init IMU_CTRL2_G
    i2c_master_start();
    i2c_master_send(IMU_ADDR | 0);      // set the address w/ write
    i2c_master_send(IMU_CTRL2_G);       // location of CTRL2_G register
    i2c_master_send(0x88);              // 1.66kHz/1000DPS
    i2c_master_stop();                  // send a stop
    
    // init IMU_CTRL3_C
    i2c_master_start();
    i2c_master_send(IMU_ADDR | 0);      // set the address w/ write
    i2c_master_send(IMU_CTRL3_C);       // location of CTRL3_C register
    i2c_master_send(0x04);              // set IF_INC to 1 to read from multiple registers
    i2c_master_stop();                  // send a stop

    return who;

}

//void imu_read(unsigned char reg, signed short * data, int len){
//    
//    // read multiple from the imu, each data takes 2 reads so you need len*2 chars
//    
//    
//    // turn the chars into the shorts
//}

void imu_read_multiple(unsigned char add, 
                       unsigned char reg, 
                       signed char *data, 
                       int length) {
    
    int i;
    i2c_master_start();
    i2c_master_send(add | 0);   // R/W = 0 = write
    i2c_master_send(reg);  // 0x20 = OUT_TEMP_L
    i2c_master_restart(); 
    i2c_master_send(add | 1); // R/W = 1 = read
    for(i=0;i<length;i++){
        data[i] = i2c_master_recv(); // receive a byte from GP7
        if(i == length-1) {
            i2c_master_ack(1);          // make the ack bit
        }
        else {
            i2c_master_ack(0);          // make the ack bit
        }
    }
    i2c_master_stop();
    
}

short getTemp(signed char *data) {
    return ((data[1]<<8 | data[0]));
}

short getGyroX(signed char *data) {
    return (data[3]<<8 | data[2]);
}

short getGyroY(signed char *data) {
    return (data[5]<<8 | data[4]);
}

short getGyroZ(signed char *data) {
    return (data[7]<<8 | data[6]);
}

short getXLX(signed char *data) {
    return (data[9]<<8 | data[8]);
}

short getXLY(signed char *data) {
    return (data[11]<<8 | data[10]);
}

short getXLZ(signed char *data) {
    return (data[13]<<8 | data[12]);
}
