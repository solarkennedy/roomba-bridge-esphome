#include "esphome/core/log.h"
//#define USE_MQTT 1

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "irobot_bridge.h"

namespace esphome {
namespace irobot_bridge {

static const char *const TAG = "Irobot_Bridge";

//float Irobot_Bridge::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }

void Irobot_Bridge::setup() {
    this->mqtt_client_ = new esphome::mqtt::MQTTClientComponent();
    this->mqtt_client_->set_broker_address(this->address_);
    this->mqtt_client_->set_username(this->blid_);
    this->mqtt_client_->set_password(this->password_);
    this->mqtt_client_->set_keep_alive(60);
    this->mqtt_client_->setup();
    return;
}

void Irobot_Bridge::dump_config() {
  ESP_LOGCONFIG(TAG, "iRobot Bridge");
}

void Irobot_Bridge::loop() {
/*
    // Handle MQTT tasks
    if (this->mqtt_client_->connected()) {
      // You can publish or subscribe here
    } else {
      // Try reconnecting if needed
    }
*/
  return;
}

void Irobot_Bridge::connect() {
  //ESP_LOGI(TAG, "[%s] Connecting", to_string(this->address_).c_str());
  return;
  // this->state_ = MYHOMEIOT_CONNECTING;
  // if (auto status = esp_ble_gattc_open(ble_host_->gattc_if, this->remote_bda_, BLE_ADDR_TYPE_PUBLIC, true))
  // {
  //   ESP_LOGW(TAG, "[%s] open error, status (%d)", to_string(this->address_).c_str(), status);
  //   report_error(MYHOMEIOT_IDLE);
  // }
}

void Irobot_Bridge::disconnect() {
  //ESP_LOGI(TAG, "[%s] Disconnecting", to_string(this->address_).c_str());
  // this->state_ = MYHOMEIOT_IDLE;
  // if (auto status = esp_ble_gattc_close(ble_host_->gattc_if, this->conn_id_))
  //   ESP_LOGW(TAG, "[%s] close error, status (%d)", to_string(this->address_).c_str(), status);
  return;
}

void Irobot_Bridge::update() {
  //this->is_update_requested_ = true;
}

// void Irobot_Bridge::report_error(esp32_ble_tracker::ClientState state) {
//   this->state_ = state;
//   this->status_set_warning();
// }

// bool Irobot_Bridge::parse_device(const esp32_ble_tracker::ESPBTDevice &device) {
//   if (!this->is_update_requested_ || this->state_ != MYHOMEIOT_IDLE
//     || device.address_uint64() != this->address_)
//     return false;

//   ESP_LOGD(TAG, "[%s] Found device", device.address_str().c_str());
//   memcpy(this->remote_bda_, device.address(), sizeof(this->remote_bda_));
//   this->state_ = MYHOMEIOT_DISCOVERED;
//   return true;
// }

// std::string Irobot_Bridge::to_string(uint64_t address) const {
//   char buffer[20];
//   sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", (uint8_t) (address >> 40), (uint8_t) (address >> 32),
//     (uint8_t) (address >> 24), (uint8_t) (address >> 16), (uint8_t) (address >> 8), (uint8_t) (address >> 0));
//   return std::string(buffer);
// }


}  // namespace myhomeiot_ble_client
}  // namespace esphome
