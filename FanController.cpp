/**
 * File FanController.cpp
 */

#include "FanController.h"
#include <thread>
#include <optional>
#include <iostream>

FanController::FanController(VoltageSensorInterface &temp_sensor_raw,
                             GpioOutputInterface &sensor_power_enable,
                             GpioOutputInterface &fan_relay_enable,
                             PwmOutputInterface &fan_output_raw) : m_temp_sensor_raw{temp_sensor_raw},
                                                                   m_sensor_power_enable{sensor_power_enable},
                                                                   m_fan_relay_enable{fan_relay_enable},
                                                                   m_fan_output_raw{fan_output_raw}
{
    std::cout << "Constructor called" << std::endl;
}

void FanController::init()
{
    m_sensor_power_enable.setOutput(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

void FanController::loop()
{
    constexpr int enable_fan_relay_threshold_temp = 60;
    constexpr int disable_fan_relay_threshold_temp = 50;
    if (m_temp_sensor_raw.getVoltage() > enable_fan_relay_threshold_temp)
    {
        // enable fan control relay
        m_fan_relay_enable.setOutput(true);
        m_fan_output_raw.setOutputDuty(computePmwOutput());
    }
    else if (m_temp_sensor_raw.getVoltage() < disable_fan_relay_threshold_temp)
    {
        // disable fan control relay
        m_fan_relay_enable.setOutput(false);
    }
    else
    {
        // TODO: Clarification What should we do if temperature is between 50-60.
        // for now do thing. Is this the expected behavior?
        // ; never reach here
    }
}

float FanController::computePmwOutput()
{
    float curTemp = m_temp_sensor_raw.getVoltage();

    if (curTemp >= table[0].first && curTemp <= table[0].second)
    {
        return (curTemp - 50) / 2;
    }
    else if (curTemp >= table[1].first && curTemp <= table[1].second)
    {
        return (curTemp - 98) * 2 + 20;
    }
    else if (curTemp >= table[2].first && curTemp <= table[2].second)
    {
        return 90.0;
    }
    else if (curTemp > table[2].second)
    {
        return 100.0;
    }
    // error case
    return -1.0;
}
