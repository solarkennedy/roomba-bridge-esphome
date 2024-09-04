#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/mqtt/mqtt_client.h"

namespace esphome {
namespace irobot_bridge {

class Irobot_Bridge : public PollingComponent {
 public:
  void setup() override;
  void dump_config() override;
  void loop() override;

  void connect();
  void disconnect();
  void update();

  void set_address(uint64_t address) { address_ = address; }
  void set_blid(const std::string &blid) { blid_ = blid; }
  void set_password(const std::string &password) { password_ = password; }

 protected:
  std::string address_;
  std::string blid_;
  std::string password_;
  esphome::mqtt::MQTTClientComponent *mqtt_client_;

};

// class MyHomeIOT_BLEClientValueTrigger : public Trigger<std::vector<uint8_t>, const MyHomeIOT_BLEClient &> {
//  public:
//   explicit MyHomeIOT_BLEClientValueTrigger(MyHomeIOT_BLEClient *parent) {
//     parent->add_on_state_callback([this](std::vector<uint8_t> value, const MyHomeIOT_BLEClient &xthis) { this->trigger(value, xthis); });
//   }
// };

}  // namespace irobot_bridge
}  // namespace esphome

