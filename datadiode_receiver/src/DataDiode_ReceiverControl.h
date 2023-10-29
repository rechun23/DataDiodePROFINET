#ifndef DATADIODE_RECEIVER_H
#define DATADIODE_RECEIVER_H

#pragma once

#include "Profinet.h"
#include "i2c/i2c.hpp"
#include <chrono>

class DataDiode_Receiver final
{
public:
    DataDiode_Receiver();
    ~DataDiode_Receiver();
    
    DataDiode_Receiver (const DataDiode_Receiver&) = delete;
    DataDiode_Receiver& operator= (const DataDiode_Receiver&) = delete;

    bool InitializeProfinet();
    bool StartProfinet();
    bool ExportGDSML(const char* filename) const;
    void RunController();
private:
    bool profinetInitialized{false};
    profinet::Profinet profinet;
    I2CReceiver receiver;
    // Data for Callback
    uint16_t status_v;
    uint16_t temp_v;
    uint16_t tank_v;
    float fRate_v;
    float pressure_v;

    bool consistencycheck;

    static const constexpr uint16_t  statusModuleID{0x00000040};
    static const constexpr uint16_t  temperatureModuleID{0x00000041};
    static const constexpr uint16_t  tankModuleID{0x00000042};
    static const constexpr uint16_t  flowRateModuleID{0x00000043};
    static const constexpr uint16_t  pressureModuleID{0x00000044};

    std::unique_ptr<profinet::ProfinetControl> profinetInstance;
};

#endif
