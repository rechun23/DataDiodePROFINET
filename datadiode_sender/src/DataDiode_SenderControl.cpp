#include "DataDiode_SenderControl.h"
#include "Device.h"
#include "gsdmltools.h"
#include <cstdint>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

DataDiode_Sender::DataDiode_Sender() : profinet(), sender(0x03, "/dev/i2c-1"), status_v(0), temp_v(0), tank_v(0), fRate_v(0.0F), pressure_v(0.0F), status_flag(false), temp_flag(false), tank_flag(false), fRate_flag(false), pressure_flag(false){}

DataDiode_Sender::~DataDiode_Sender(){}

bool DataDiode_Sender::InitializeProfinet()
{
    profinet::Device& device = profinet.GetDevice();

    device.properties.vendorName = "FH Technikum Wien";
    // ID of the device, which is unique for the given vendor.
    device.properties.deviceID = 0x0001;
    device.properties.deviceName = "Datadiodesender";
    device.properties.deviceInfoText = "Simulate profinet modul which is part of a data diode";
    device.properties.deviceProductFamily = "datadiode";
    // profinet name
    device.properties.stationName = "datadiodesender";        
    device.properties.numSlots = 7;
    
    // Current software version of device.
    device.properties.swRevMajor = 0;
    device.properties.swRevMinor = 1;
    device.properties.swRevPatch = 0;
    // Current hardware version of device.
    device.properties.hwRevMajor = 1;
    device.properties.hwRevMinor = 0;

    /* Note: You need to read out the actual hardware serial number instead */
    device.properties.serialNumber = "000-000-000";

    /* GSDML tag: OrderNumber */
    device.properties.orderID = "123 444 555";

    /* GSDML tag: ModuleInfo / Name */
    device.properties.productName = "datadiodesender";

    /* GSDML tag: MinDeviceInterval */
    device.properties.minDeviceInterval = 8*32; /* 8*1 ms */
    device.properties.defaultMautype = 0x10; /* Copper 100 Mbit/s Full duplex */

 
    // Status indicator
    auto statusModuleWithPlugInfo = device.modules.Create(statusModuleID,1);
    if(!statusModuleWithPlugInfo)
        return false;
    auto& [statusPlugInfo, statusModule]{*statusModuleWithPlugInfo};
    statusModule.properties.name = "Status module";
    statusModule.properties.infoText = "Modul for Status signals";
    profinet::Submodule* statusSubmodule = statusModule.submodules.Create(0x00000140);
    statusSubmodule->properties.name = "Status Submodule";
    statusSubmodule->properties.infoText = "Signal if there is a problem in processings station";
    auto statusSetCallback = [this](uint16_t value) -> void
    {
            if(status_v != value){
                status_flag = true;
            }
            status_v = value;     
    };
    statusSubmodule->inputs.Create<uint16_t, sizeof(uint16_t)>(statusSetCallback);
    // Get is from Rpi to PLC
    // Set is from PLC to Rpi

    // Temperature Module
    auto tempModuleWithPlugInfo = device.modules.Create(temperatureModuleID);
    if(!tempModuleWithPlugInfo)
        return false;
    auto& [tempPlugInfo, tempModule]{*tempModuleWithPlugInfo};
    tempModule.properties.name = "Temperature Module";
    tempModule.properties.infoText = "Module for Temperature threshold";
    profinet::Submodule* tempSubmodule = tempModule.submodules.Create(0x00000141);
    tempSubmodule->properties.name = "Temperature submodule";
    tempSubmodule->properties.infoText = "For general temperature control purpose";
    auto tempSetCallback = [this](uint16_t value) -> void
    {
        if(temp_v != value){
            temp_v = value;
            temp_flag = true;
        }
        
    };
    tempSubmodule->inputs.Create<uint16_t, sizeof(uint16_t)>(tempSetCallback);

    // Tank Level
    auto tankModuleWithPlugInfo = device.modules.Create(tankModuleID);
    if(!tankModuleWithPlugInfo)
        return false;
    auto& [tankPlugInfo, tankModule]{*tankModuleWithPlugInfo};
    tankModule.properties.name = "Tank Module";
    tankModule.properties.infoText = "Module for Tank level";
    profinet::Submodule* tankSubmodule = tankModule.submodules.Create(0x00000142);
    tankSubmodule->properties.name = "tank submodule";
    tankSubmodule->properties.infoText = "Submodul for tank level";
    auto tankSetCallback = [this](uint16_t value) -> void
    {
        if(tank_v != value){
            tank_v = value;
            tank_flag = true;
        }
    };
    tankSubmodule->inputs.Create<uint16_t, sizeof(uint16_t)>(tankSetCallback);

    // Flow Rate
    auto fRateModuleWithPlugInfo = device.modules.Create(flowRateModuleID);
    if(!fRateModuleWithPlugInfo)
        return false;
    auto& [fRatePlugInfo, fRateModule]{*fRateModuleWithPlugInfo};
    fRateModule.properties.name = "Flow rate Module";
    fRateModule.properties.infoText = "For Flow rate threshold";
    profinet::Submodule* fRateSubmodule = fRateModule.submodules.Create(0x00000143);
    fRateSubmodule->properties.name = "Flow rate Submodule";
    fRateSubmodule->properties.infoText = "Submodul for flow rate";
    auto fRateSetCallback = [this](float value) -> void
    {
        if(fRate_v != value){
            fRate_v = value;
            fRate_flag = true;
        }
    };
    fRateSubmodule->inputs.Create<float, sizeof(float)>(fRateSetCallback);
    
    // Pressure
    auto pressureModuleWithPlugInfo = device.modules.Create(pressureModuleID);
    if(!pressureModuleWithPlugInfo)
        return false;
    auto& [pressurePlugInfo, pressureModule]{*pressureModuleWithPlugInfo};
    pressureModule.properties.name = "Pressure Module";
    pressureModule.properties.infoText = "For pressure threshold";
    profinet::Submodule* pressureSubmodule = pressureModule.submodules.Create(0x00000144);
    pressureSubmodule->properties.name = "Pressure Submodule";
    pressureSubmodule->properties.infoText = "Submodul for presure";
    auto pressureSetCallback = [this](float value) -> void
    {
        if(pressure_v != value){
            pressure_v = value;
            pressure_flag = true;
        }
    };
    pressureSubmodule->inputs.Create<float, sizeof(float)>(pressureSetCallback);
    profinetInitialized = true;
    return true;
}

bool DataDiode_Sender::StartProfinet()
{
    if(!profinetInitialized)
        return false;
    profinetInstance = profinet.Initialize();
    if(!profinetInstance)
        return false;
    return profinetInstance->Start();
}

void DataDiode_Sender::RunController()
{
    if(sender.init()){
		std::cout << "I2C init succesful!" << std::endl;
	}else{exit(-1);}
    // Create a vector to hold your data
    std::vector<uint8_t> data;

    // Send Module information for check
    data.push_back(static_cast<uint8_t>((statusModuleID >> 8) & 0xFF));
    data.push_back(static_cast<uint8_t>(statusModuleID & 0xFF));
    data.push_back(static_cast<uint8_t>((temperatureModuleID >> 8) & 0xFF));
    data.push_back(static_cast<uint8_t>(temperatureModuleID & 0xFF));
    data.push_back(static_cast<uint8_t>((tankModuleID >> 8) & 0xFF));
    data.push_back(static_cast<uint8_t>(tankModuleID & 0xFF));
    data.push_back(static_cast<uint8_t>((flowRateModuleID >> 8) & 0xFF));
    data.push_back(static_cast<uint8_t>(flowRateModuleID & 0xFF));
    data.push_back(static_cast<uint8_t>((pressureModuleID >> 8) & 0xFF));
    data.push_back(static_cast<uint8_t>(pressureModuleID & 0xFF));

    if(sender.sendData(data)){
        std::cout << "Sending all modules succecfull!" << std::endl;
        data.clear();
    }else{
        std::cout << "Something didn't work!" << std::endl;
        exit(-1);
    }

    auto startTime = std::chrono::steady_clock::now() - std::chrono::seconds(10);
    while(true)
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedDuration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
        if(elapsedDuration.count() >= 10 || status_flag){
            data.push_back(static_cast<uint8_t>((statusModuleID >> 8) & 0xFF));
            data.push_back(static_cast<uint8_t>(statusModuleID & 0xFF));
            data.push_back(static_cast<uint8_t>((status_v >> 8) & 0xFF));
            data.push_back(static_cast<uint8_t>(status_v & 0xFF));
            if(sender.sendData(data)){
                std::cout << "Sending status succecfull!" << std::endl;
            }else{
                std::cout << "Fail status!" << std::endl;
                break;
            }
            data.clear();
            // Reset the start time for the next 10-second interval
            startTime = std::chrono::steady_clock::now();
            status_flag = false;
        }

        if(temp_flag == true){
            data.push_back(static_cast<uint8_t>((temperatureModuleID >> 8) & 0xFF));
            data.push_back(static_cast<uint8_t>(temperatureModuleID & 0xFF));
            data.push_back(static_cast<uint8_t>((temp_v >> 8) & 0xFF));
            data.push_back(static_cast<uint8_t>(temp_v & 0xFF));
            
            if(sender.sendData(data)){
                std::cout << "Sending temperature succecfull!" << std::endl;
            }else{
                std::cout << "Fail temperature!" << std::endl;
                break;
            }
            data.clear();
            temp_flag = false;
        }
        if(tank_flag == true){
            data.push_back(static_cast<uint8_t>((tankModuleID >> 8) & 0xFF));
            data.push_back(static_cast<uint8_t>(tankModuleID & 0xFF));
            data.push_back(static_cast<uint8_t>((tank_v >> 8) & 0xFF));
            data.push_back(static_cast<uint8_t>(tank_v & 0xFF));
            
            if(sender.sendData(data)){
                std::cout << "Sending tank level succecfull!" << std::endl;
            }else{
                std::cout << "Fail tank!" << std::endl;
                break;
            }
            data.clear();
            tank_flag = false;
        }
        if(fRate_flag == true){
            data.resize(sizeof(float));
            std::memcpy(data.data(),&fRate_v,sizeof(float));
            data.insert(data.begin(), flowRateModuleID & 0xFF);
            data.insert(data.begin(), (flowRateModuleID >> 8) & 0xFF);
            if(sender.sendData(data)){
                std::cout << "Sending temperature succecfull!" << std::endl;
            }else{
                std::cout << "Error flow!" << std::endl;
                break;
            }
            data.clear();
            fRate_flag = false;
        }
        if(pressure_flag == true){
            data.resize(sizeof(float));
            std::memcpy(data.data(),&pressure_v,sizeof(float));
            data.insert(data.begin(), pressureModuleID & 0xFF);
            data.insert(data.begin(), (pressureModuleID >> 8) & 0xFF);
            if(sender.sendData(data)){
                std::cout << "Sending pressure succecfull!" << std::endl;
            }else{
                std::cout << "Error pressure!" << std::endl;
                break;
            }
            data.clear();
            pressure_flag = false;
        }
        // busy waiting...
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    }
}

bool DataDiode_Sender::ExportGDSML(const char* folderName) const{
    std::string folder{folderName};
    return profinet::gsdml::CreateGsdml(profinet, folder);
}
