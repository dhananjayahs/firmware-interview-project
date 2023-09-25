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
   std::cout<<"Constructor called"<<std::endl;
}

void FanController::init() {
    m_sensor_power_enable.setOutput(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

void FanController::loop() {
    if(m_temp_sensor_raw.getVoltage() > 60) {
        // enable fan control relay
        m_fan_relay_enable.setOutput(true);
        m_fan_output_raw.setOutputDuty(computePmwOutput());
    }
    else if(m_temp_sensor_raw.getVoltage() < 50) {
        // disable fan control relay
        m_fan_relay_enable.setOutput(false);
    }
    else {
        // do nothing; never reach here
    }
}

float FanController::computePmwOutput() {
    float curTemp = m_temp_sensor_raw.getVoltage();

    if(curTemp >= 50 && curTemp <= 99) {
        return (curTemp - 50)/2;
    }
    else if(curTemp >= 100 && curTemp <= 129) {
        return (curTemp - 98)*2 + 20;
    }
    else if(curTemp >= 130 && curTemp <= 140) {
        return 90.0;
    } else if(curTemp > 140) {
        return 100.0;
    }
    // error case
    return -1.0;
}