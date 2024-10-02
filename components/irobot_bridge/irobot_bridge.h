#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "esphome/components/sensor/sensor.h"
#include "ca_cert_chain.h"

namespace esphome
{
  namespace irobot_bridge
  {

    class Irobot_Bridge : public Component
    {
    public:
      void setup() override;
      void dump_config() override;
      void loop() override;
      float get_setup_priority() const override;

      void connect();
      void onMqttConnect(bool sessionPresent);
      void disconnect();
      void onMqttDisconnect(esphome::mqtt::MQTTClientDisconnectReason reason);

      void set_address(const std::string &address) { this->address_ = address; }
      void set_blid(const std::string &blid) { this->blid_ = blid; }
      void set_password(const std::string &password) { this->password_ = password; }

      void handle_json_message(const std::string &topic, const JsonObject doc);
      void handle_message(const std::string &topic, const std::string &payload);

      void set_battery_percent_sensor(sensor::Sensor *sensor) { this->battery_percent = sensor; }

    protected:
      std::string address_;
      std::string blid_;
      std::string password_;
      esphome::mqtt::MQTTClientComponent *mqtt_client_;

      sensor::Sensor *battery_percent{nullptr};
    };

  } // namespace irobot_bridge
} // namespace esphome
