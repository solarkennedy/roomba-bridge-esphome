#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/button/button.h"
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
      void handle_wifistat_json_message(const std::string &topic, const JsonObject doc);
      void handle_message(const std::string &topic, const std::string &payload);

      void set_battery_percent_sensor(sensor::Sensor *sensor) { this->battery_percent = sensor; }
      void set_rssi_sensor(sensor::Sensor *sensor) { this->rssi_sensor = sensor; }

      void set_start_roomba_button(button::Button *button) { this->start_roomba_button_ = button; };
      void set_stop_roomba_button(button::Button *button) { this->stop_roomba_button_ = button; };
      void set_pause_roomba_button(button::Button *button) { this->pause_roomba_button_ = button; };
      void set_resume_roomba_button(button::Button *button) { this->resume_roomba_button_ = button; };

      button::Button *start_roomba_button_{nullptr};
      button::Button *stop_roomba_button_{nullptr};
      button::Button *pause_roomba_button_{nullptr};
      button::Button *resume_roomba_button_{nullptr};

      void pause_roomba_action();
      void start_roomba_action();
      void resume_roomba_action();
      void stop_roomba_action();

      bool api_call_cmd(const char *command);
      bool api_call(const char *command, JsonObject &additionalArgs);

    protected:
      std::string address_;
      std::string blid_;
      std::string password_;
      esphome::mqtt::MQTTClientComponent *mqtt_client_;

      sensor::Sensor *battery_percent{nullptr};
      sensor::Sensor *rssi_sensor{nullptr};
    };

  } // namespace irobot_bridge
} // namespace esphome
