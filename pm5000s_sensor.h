#pragma once

#include "esphome.h"

namespace esphome {
namespace pm5000s_sensor {

class PM5000SSensor : public PollingComponent, public i2c::I2CDevice {
 public:
  explicit PM5000SSensor(i2c::I2CComponent *parent) : PollingComponent(2000), i2c::I2CDevice(parent, 0x50) {}

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void update() override;

  // Particle sensors exposed to ESPHome
  Sensor *pm_0_3 = new Sensor();
  Sensor *pm_0_5 = new Sensor();
  Sensor *pm_1_0 = new Sensor();
  Sensor *pm_2_5 = new Sensor();
  Sensor *pm_5_0 = new Sensor();
  Sensor *pm_10 = new Sensor();

  // Control commands
  void start_measurement();
  void stop_measurement();
  void set_output_unit(uint8_t unit);  // 0: pcs/L, 1: pcs/m³, 2: pcs/28.3L

 protected:
  void read_data_();
  void send_command_(const std::vector<uint8_t> &cmd);
  uint8_t calculate_checksum_(const std::vector<uint8_t> &data);

};

}  // namespace pm5000s_sensor
}  // namespace esphome
