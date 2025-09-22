#include "pm5000s_sensor.h"

namespace esphome {
namespace pm5000s_sensor {

void PM5000SSensor::setup() {
  ESP_LOGCONFIG("pm5000s_sensor", "Setting up PM5000S sensor...");
  start_measurement();
}

void PM5000SSensor::dump_config() {
  ESP_LOGCONFIG("pm5000s_sensor", "PM5000S Sensor:");
  LOG_I2C_DEVICE(this);
  LOG_SENSOR("  ", "PM >0.3μm", pm_0_3);
  LOG_SENSOR("  ", "PM >0.5μm", pm_0_5);
  LOG_SENSOR("  ", "PM >1.0μm", pm_1_0);
  LOG_SENSOR("  ", "PM >2.5μm", pm_2_5);
  LOG_SENSOR("  ", "PM >5.0μm", pm_5_0);
  LOG_SENSOR("  ", "PM >10μm", pm_10);
}

void PM5000SSensor::update() {
  read_data_();
}

void PM5000SSensor::send_command_(const std::vector<uint8_t> &cmd) {
  // Write the full command frame to the sensor over I2C
  if (!write_bytes(cmd.data(), cmd.size())) {
    ESP_LOGW("pm5000s_sensor", "Failed to send command to PM5000S sensor");
  }
}

uint8_t PM5000SSensor::calculate_checksum_(const std::vector<uint8_t> &data) {
  uint8_t cs = 0;
  for (size_t i = 0; i < data.size(); i++) {
    cs ^= data[i];
  }
  return cs;
}

void PM5000SSensor::start_measurement() {
  std::vector<uint8_t> cmd = {0x16, 0x07, 0x02, 0x00, 0x00, 0x00};
  uint8_t cs = calculate_checksum_(cmd);
  cmd.push_back(cs);

  std::vector<uint8_t> frame;
  frame.push_back(0x50);  
  frame.insert(frame.end(), cmd.begin(), cmd.end());

  send_command_(frame);
}

void PM5000SSensor::stop_measurement() {
  std::vector<uint8_t> cmd = {0x16, 0x07, 0x01, 0x00, 0x00, 0x00};
  uint8_t cs = calculate_checksum_(cmd);
  cmd.push_back(cs);

  std::vector<uint8_t> frame;
  frame.push_back(0x50);  
  frame.insert(frame.end(), cmd.begin(), cmd.end());

  send_command_(frame);
}

void PM5000SSensor::set_output_unit(uint8_t unit) {
  std::vector<uint8_t> cmd = {0x16, 0x07, 0x08, 0x00, 0x00, unit};
  uint8_t cs = calculate_checksum_(cmd);
  cmd.push_back(cs);

  std::vector<uint8_t> frame;
  frame.push_back(0x50);
  frame.insert(frame.end(), cmd.begin(), cmd.end());

  send_command_(frame);
}

void PM5000SSensor::read_data_() {
  const uint8_t read_address = 0x51;  // Read address

  uint8_t buffer[32];
  // Read 32 bytes from sensor
  if (!read_bytes(buffer, sizeof(buffer))) {
    ESP_LOGW("pm5000s_sensor", "Failed to read data from PM5000S sensor");
    return;
  }

  if (buffer[0] != 0x16) {
    ESP_LOGW("pm5000s_sensor", "Invalid frame header: 0x%02X", buffer[0]);
    return;
  }

  if (buffer[1] != 0x20) {
    ESP_LOGW("pm5000s_sensor", "Unexpected frame length: 0x%02X", buffer[1]);
    return;
  }

  uint8_t checksum = 0;
  for (int i = 0; i < 31; i++) {
    checksum ^= buffer[i];
  }
  if (checksum != buffer[31]) {
    ESP_LOGW("pm5000s_sensor", "Checksum mismatch: calculated 0x%02X, expected 0x%02X", checksum, buffer[31]);
    return;
  }

  auto parse_uint32 = [&](int start_index) -> uint32_t {
    return ((uint32_t)buffer[start_index] << 24) |
           ((uint32_t)buffer[start_index + 1] << 16) |
           ((uint32_t)buffer[start_index + 2] << 8) |
           ((uint32_t)buffer[start_index + 3]);
  };

  pm_0_3->publish_state(parse_uint32(7));   // P8-P11 >0.3μm
  pm_0_5->publish_state(parse_uint32(11));  // P12-P15 >0.5μm
  pm_1_0->publish_state(parse_uint32(15));  // P16-P19 >1.0μm
  pm_2_5->publish_state(parse_uint32(19));  // P20-P23 >2.5μm
  pm_5_0->publish_state(parse_uint32(23));  // P24-P27 >5.0μm
  pm_10->publish_state(parse_uint32(27));   // P28-P31 >10μm
}

}  
}  
