#ifndef I2C_H
#define I2C_H

#pragma once

// For General Purpose
#include <iostream>
#include <cstring>
#include <vector>

// For Sender
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// For Receiver
#include <pigpio.h>


class I2CSender{
public:
    I2CSender(int address, const char *file);
    ~I2CSender();
    bool init();
    bool sendData(const std::vector<uint8_t> &data);

private:
    int i2c_fd;
    int i2c_address;
    const char *i2c_file;
};



class I2CReceiver{
public:
    I2CReceiver(int address);
    ~I2CReceiver();
    bool init();
    void receiveData();

private:
    int address;
    bsc_xfer_t xfer;
    int getControlBits(int address, bool open);
    void processData(const char* data, int length);
};


#endif // I2C_H
