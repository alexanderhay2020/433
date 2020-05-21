#ifndef IMU_H__
#define IMU_H__

#include<xc.h> // processor SFR definitions

#define IMU_WHOAMI 0x0F         // 0000 1111
#define IMU_ADDR 0xD6           // 1101 0110
#define IMU_CTRL1_XL 0x10
#define IMU_CTRL2_G 0x11
#define IMU_CTRL3_C 0x12
#define IMU_OUT_TEMP_L 0x20

char imu_setup();
void imu_read_multiple(unsigned char, unsigned char, signed char *, int);

short getTemp(signed char *);
short getGyroX(signed char *);
short getGyroY(signed char *);
short getGyroZ(signed char *);
short getXLX(signed char *);
short getXLY(signed char *);
short getXLZ(signed char *);

#endif