#include "i2c.hpp"


//---------------------------------------------------------------------//
//------------I2C sender using basic file I/O operations---------------//
//---------------------------------------------------------------------//

I2CSender::I2CSender(int address, const char *file):i2c_fd(-1), i2c_address(address), i2c_file(file){}

I2CSender::~I2CSender() {
    if (i2c_fd != -1) {
        close(i2c_fd);
        i2c_fd = -1;
    }
}

bool I2CSender::init() {
    i2c_fd = open(i2c_file, O_RDWR);
    if (i2c_fd == -1){
        std::cerr << "Error opening I2C bus!" << std::endl;
        return false;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, i2c_address) < 0) {
        std::cerr << "Error setting I2C address!" << std::endl;
        close(i2c_fd);
        return false;
    }
    return true;
}

bool I2CSender::sendData(const std::vector<uint8_t> &data){
    size_t bytes_written = write(i2c_fd, data.data(), data.size());
    if (bytes_written != data.size()) {
        std::cerr << "Error writing to I2C bus!" << std::endl;
        return false;
    }
    return true;
}





//---------------------------------------------------------------------//
//I2C using pigpio lib because pi can't act as slave without workaround//
//---------------------------------------------------------------------//

I2CReceiver::I2CReceiver(int address):address(address){}

bool I2CReceiver::init(){
    // Initialize GPIOs and set up I2C slave
    if(gpioInitialise() < 0){
        std::cerr << "Failed to open init gpio!" << std::endl;
        return false;
    }
    xfer.control = getControlBits(address, true);
    int status = bscXfer(&xfer);
    if(status < 0){
        std::cerr << "Failed to open I2C slave!" << std::endl;
        return false;
    }
    return true;
}

// Close I2C slave
I2CReceiver::~I2CReceiver(){
    xfer.control = getControlBits(address, false);
    bscXfer(&xfer);
    gpioTerminate();
}

void I2CReceiver::receiveData(std::vector<uint8_t>& data){
    xfer.rxCnt = 0;
    bscXfer(&xfer);
    if(xfer.rxCnt > 0){
        std::cout << "Data received!" << std::endl;
        std::cout << "Received " << xfer.rxCnt << " bytes: ";
        for(int i = 0; i < xfer.rxCnt; i++) {
            std::cout << static_cast<int>(xfer.rxBuf[i]) << " ";
            data.push_back(xfer.rxBuf[i]);
        }
        std::cout << "\n";
    }
}

int I2CReceiver::getControlBits(int address, bool open){
    // http://abyz.me.uk/rpi/pigpio/cif.html#bscXfer
    int flags;
    if(open)
        flags = (1 << 9) | (1 << 8) | (1 << 2) | (1 << 0);
    else // Close
        flags = (1 << 7) | (0 << 2) | (0 << 0);
    return (address << 16) | flags;

    // 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
    //  a  a  a  a  a  a  a  -  - IT HC TF IR RE TE BK EC ES PL PH I2 SP EN
    /*  aaaaaaa	defines the I2C slave address (only relevant in I2C mode)
        IT	invert transmit status flags
        HC	enable host control
        TF	enable test FIFO
        IR	invert receive status flags
        RE	enable receive
        TE	enable transmit
        BK	abort operation and clear FIFOs
        EC	send control register as first I2C byte
        ES	send status register as first I2C byte
        PL	set SPI polarity high
        PH	set SPI phase high
        I2	enable I2C mode
        SP	enable SPI mode
        EN	enable BSC peripheral
    */
}

void I2CReceiver::processData(const char* data, int length){
    // lala
}
