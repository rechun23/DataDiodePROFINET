# DataDiodePROFINET
A secure unidirectional communication in PROFINET network with implementation of a data diode.

## Overview
This repository showcases the development of a data diode system designed for more security in PROFINET-based industrial automation. The project focuses on achieving robust unidirectional communication and includes features such as a consistency check, selective parameter transmission and continuous status updates.

## Dependencies for this project
This project relies on the following dependencies for seamless operation:

1. **Profipp:** A lightweight C++ API based on P-net, facilitating communication between devices via PROFINET. Find the required repository [here](https://github.com/langmo/profipp/).

   ```bash
   git clone https://github.com/langmo/profipp.git

Follow the installation instructions in the Profipp repository.

2. **pigpio:** Pigpio is responsible for I2C communication and GPIO handling on the Raspberry Pi. Find the required repository [here](https://github.com/joan2937/pigpio).

    ```bash
    sudo apt-get update
    sudo apt-get install pigpio

Ensure pigpio is running as a daemon:
    
    ```bash
    sudo pigpiod

For more information, refer to the pigpio [documentation](http://abyz.me.uk/rpi/pigpio/).

These dependencies must be installed and configured before running the Data Diode PROFINET system. Make sure to follow the respective documentation for each dependency to ensure a smooth setup process.

## Installation

### Raspberry Pi
1. Clone the repository:
    ```bash
    git clone https://github.com/rechun23/DataDiodePROFINET.git
    ```

2. Navigate to the respective folder for the Raspberry Pi:
   - Sender: `datadiode_sender`
   - Receiver: `datadiode_receiver`

3. Run the build and install script:
    ```bash
    cd datadiode_sender  # or datadiode_receiver
    ./build_and_install_release.sh
    ```

4. Start the Data Diode application:
   - Sender: `sudo ./datadiode_sender -s`
   - Receiver: `sudo ./datadiode_receiver -s`

### PLC
1. Install the GSDML file using the GSD Importer in TIA Portal:
    - For Receiver: Import GSDML into `datadiode_receiver/gsdml`
    - For Sender: Import GSDML into `datadiode_sender/gsdml`

2. Access the sample TIA project:
    - For Receiver: Navigate to `datadiode_receiver/resources`
    - For Sender: Navigate to `datadiode_sender/resources`

Follow these installation steps to set up the Data Diode PROFINET system on Raspberry Pi and PLC.


