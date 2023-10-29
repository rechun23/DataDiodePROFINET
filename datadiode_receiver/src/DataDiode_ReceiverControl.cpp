#include "DataDiode_ReceiverControl.h"
#include "Device.h"
#include "gsdmltools.h"
#include <cstdint>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

DataDiode_Receiver::DataDiode_Receiver() : profinet(), receiver(0x03), status_v(0), temp_v(0), tank_v(0), fRate_v(0.0F), pressure_v(0.0F), consistencycheck(false){}

DataDiode_Receiver::~DataDiode_Receiver(){}

bool DataDiode_Receiver::InitializeProfinet()
{
    profinet::Device& device = profinet.GetDevice();

    device.properties.vendorName = "FH Technikum Wien";
    // ID of the device, which is unique for the given vendor.
    device.properties.deviceID = 0x0002;
    device.properties.deviceName = "Datadiodereceiver";
    device.properties.deviceInfoText = "Simulate profinet modul receiver which is part of a data diode";
    device.properties.deviceProductFamily = "datadiode";
    // profinet name
    device.properties.stationName = "datadiodereceiver";        
    device.properties.numSlots = 7;
    
    // Current software version of device.
    device.properties.swRevMajor = 0;
    device.properties.swRevMinor = 1;
    device.properties.swRevPatch = 0;
    // Current hardware version of device.
    device.properties.hwRevMajor = 1;
    device.properties.hwRevMinor = 0;

    /* Note: You need to read out the actual hardware serial number instead */
    device.properties.serialNumber = "000-000-001";

    /* GSDML tag: OrderNumber */
    device.properties.orderID = "123 444 777";

    /* GSDML tag: ModuleInfo / Name */
    device.properties.productName = "datadiodereceiver";

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
    // Get is from PLC to RPI
    // Set is from RPI to PLC
    
    auto statusgetCallback = [this]() -> uint16_t
    {
        return status_v;
    };
    statusSubmodule->outputs.Create<uint16_t, sizeof(uint16_t)>(statusgetCallback);


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
    auto tempGetCallback = [this]() -> uint16_t
    {
        return temp_v;
    };
    tempSubmodule->outputs.Create<uint16_t, sizeof(uint16_t)>(tempGetCallback);


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
    auto tankGetCallback = [this]() -> uint16_t
    {
        return tank_v;
    };
    tankSubmodule->outputs.Create<uint16_t, sizeof(uint16_t)>(tankGetCallback);


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
    auto fRateGetCallback = [this]() -> float
    {
        return fRate_v;
    };
    fRateSubmodule->outputs.Create<float, sizeof(float)>(fRateGetCallback);
    
    
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
    auto pressureGetCallback = [this]() -> float
    {
        return pressure_v;
    };
    pressureSubmodule->outputs.Create<float, sizeof(float)>(pressureGetCallback);

    profinetInitialized = true;
    return true;
}

bool DataDiode_Receiver::StartProfinet(){
    if(!profinetInitialized)
        return false;
    profinetInstance = profinet.Initialize();
    if(!profinetInstance)
        return false;
    return profinetInstance->Start();
}

void DataDiode_Receiver::RunController()
{
    if(receiver.init()){
		std::cout << "I2C receiver init succesful!" << std::endl;
	}else{exit(-1);}

    // Create a vector to hold your data
    std::vector<uint8_t> data;
    while(!consistencycheck){
        receiver.receiveData(data);
        if(data.size() == 10){

            uint16_t id_1 = (data[0] << 8) | data[1];
            uint16_t id_2 = (data[2] << 8) | data[3];
            uint16_t id_3 = (data[4] << 8) | data[5];
            uint16_t id_4 = (data[6] << 8) | data[7];
            uint16_t id_5 = (data[8] << 8) | data[9];
            

            if(id_1 == statusModuleID && id_2 == temperatureModuleID && id_3 == tankModuleID && id_4 == flowRateModuleID && id_5 == pressureModuleID){
                std::cout << "Sender consistency check passed!" << std::endl;
                consistencycheck = true;
            }else{
                std::cout << "Pls check Moduls!" << std::endl;
            }


        }
    }
    

    auto lastReceiveTime = std::chrono::steady_clock::now();
    while(true){
        receiver.receiveData(data);
        if(data.size() >= 2){
            uint16_t identifier = (data[0] << 8) | data[1];
            std::vector<uint8_t> payloadData(data.begin() + 2, data.end());
            switch(identifier){
                case statusModuleID: // uint16_t
                    if(payloadData.size() >= 2){
                        status_v = (payloadData[0] << 8) | payloadData[1];
                    }
                    lastReceiveTime = std::chrono::steady_clock::now();
                    break;

                case temperatureModuleID:
                    if(payloadData.size() >= 2){
                        temp_v = (payloadData[0] << 8) | payloadData[1];
                    }
                    break;

                case tankModuleID:
                    if(payloadData.size() >= 2){
                        tank_v = (payloadData[0] << 8) | payloadData[1];
                    }
                    break;
                case flowRateModuleID:
                    if(payloadData.size() >= 4) {
                        fRate_v = *reinterpret_cast<float*>(&payloadData[0]);
                    }
                    break;
                case pressureModuleID:
                    if(payloadData.size() >= 4) {
                        pressure_v = *reinterpret_cast<float*>(&payloadData[0]);
                    }
                    break;

                default:
                    std::cerr << "Data corrupted!" << std::endl;
                    exit(-1);
                    break;   
            }
        }else{
            //std::cerr << "Data Payload too small!" << std::endl;
            //exit(-1);
        }
        data.clear();
        

        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedDuration = std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastReceiveTime);
        // if over a minute not getting update from status -> exit
        if(elapsedDuration.count() >= 1){
            std::cerr << "Lost Connection to Sender!" << std::endl;
            // Emergency state here
            exit(-1);
        }

        // busy waiting...
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    }
}

bool DataDiode_Receiver::ExportGDSML(const char* folderName) const{
    std::string folder{folderName};
    return profinet::gsdml::CreateGsdml(profinet, folder);
}
