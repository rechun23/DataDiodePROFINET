# DataDiodePROFINET
A secure unidirectional communication in PROFINET network with implementation of a data diode.

## Overview
This repository showcases the development of a data diode system designed for more security in PROFINET-based industrial automation. The project focuses on achieving robust unidirectional communication and includes features such as a consistency check, selective parameter transmission and continuous status updates.

## Dependencies for This Project
This project relies on the following dependencies for seamless operation:

1. **Profipp:** A lightweight C++ API based on P-net, facilitating communication between devices via PROFINET. Find the required repository [here](https://github.com/langmo/profipp/).

   ```bash
   git clone https://github.com/langmo/profipp.git

Follow the installation instructions in the Profipp repository.

1. **pigpio:** Pigpio is responsible for I2C communication and GPIO handling on the Raspberry Pi. Find the required repository [here](https://github.com/joan2937/pigpio).

    ```bash
    sudo apt-get update
    sudo apt-get install pigpio

Ensure pigpio is running as a daemon:
    ```bash
    sudo pigpiod

For more information, refer to the pigpio [documentation](http://abyz.me.uk/rpi/pigpio/).

These dependencies must be installed and configured before running the Data Diode PROFINET system. Make sure to follow the respective documentation for each dependency to ensure a smooth setup process.
## Hardware



## Installation Raspberry PI



## Installation PLC


