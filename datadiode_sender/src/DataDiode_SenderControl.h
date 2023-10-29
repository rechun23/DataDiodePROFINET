#ifndef DATADIODE_SENDER_H
#define DATADIODE_SENDER_H

#pragma once

#include "Profinet.h"
#include "i2c/i2c.hpp"
#include <chrono>

class DataDiode_Sender final
{
public:
    DataDiode_Sender();
    ~DataDiode_Sender();
    
    DataDiode_Sender (const DataDiode_Sender&) = delete;
    DataDiode_Sender& operator= (const DataDiode_Sender&) = delete;

    bool InitializeProfinet();
    bool StartProfinet();
    bool ExportGDSML(const char* filename) const;
    void RunController();
private:
    bool profinetInitialized{false};
    profinet::Profinet profinet;
    I2CSender sender;
    // Data for Callback
    uint16_t status_v;
    uint16_t temp_v;
    uint16_t tank_v;
    // Declare as uint32_t for sending float -> uint32_t
    float fRate_v;
    float pressure_v;

    // New Data Flag
    bool status_flag;
    bool temp_flag;
    bool tank_flag;
    bool fRate_flag;
    bool pressure_flag;

    static const constexpr uint16_t  statusModuleID{0x00000040};
    static const constexpr uint16_t  temperatureModuleID{0x00000041};
    static const constexpr uint16_t  tankModuleID{0x00000042};
    static const constexpr uint16_t  flowRateModuleID{0x00000043};
    static const constexpr uint16_t  pressureModuleID{0x00000044};


    std::unique_ptr<profinet::ProfinetControl> profinetInstance;
};

#endif
