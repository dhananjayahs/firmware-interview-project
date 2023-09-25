#pragma once

#include "Drivers.h"
#include <vector>
/**
 * @brief Example FanController base
 */
class FanController
{

private:
  VoltageSensorInterface &m_temp_sensor_raw;
  GpioOutputInterface &m_sensor_power_enable;
  GpioOutputInterface &m_fan_relay_enable;
  PwmOutputInterface &m_fan_output_raw;

  // table with ranges
  std::vector<std::pair<int, int>> table{{50, 99},
                                         {100, 129},
                                         {130, 140}};

  /**
   * @brief method to compute PMW output
   */
  float computePmwOutput();

public:
  /*
   * @brief constructor for FanContoller
   */
  FanController(VoltageSensorInterface &temp_sensor_raw,
                GpioOutputInterface &sensor_power_enable,
                GpioOutputInterface &fan_relay_enable,
                PwmOutputInterface &fan_output_raw);
  /*
   * @brief destructor for FanContoller
   */
  virtual ~FanController(){};
  /*
   * @brief One time executed initialization method
   */
  void init();
  /**
   * @brief Periodic loop method that will be called every 500ms
   */
  void loop();
};
