#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/button/button.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
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
      void set_pmap_id(std::string pmap_id) { this->pmap_id_ = pmap_id; }
      void set_user_pmapv_id(std::string user_pmapv_id) { this->user_pmapv_id_ = user_pmapv_id; }
      void add_region(std::string region_id, std::string region_name, std::string region_type, std::string type)
      {
        if (!region_id.empty())
        {
          Region region = {region_id, region_name, region_type, type};
          this->regions_.push_back(region);
        }
      }

      void handle_update_message(const std::string &topic, const std::string &doc);
      void handle_wifistat_message(const std::string &topic, const std::string &doc);

      void set_battery_percent_sensor(sensor::Sensor *sensor) { this->battery_percent = sensor; }
      void set_rssi_sensor(sensor::Sensor *sensor) { this->rssi_sensor = sensor; }
      void set_cleaning_phase_sensor(text_sensor::TextSensor *sensor) { this->cleaning_phase_sensor = sensor; }
      void set_bin_full_sensor(binary_sensor::BinarySensor *sensor) { this->bin_full_sensor = sensor; }

      void set_start_roomba_button(button::Button *button) { this->start_roomba_button_ = button; };
      void set_stop_roomba_button(button::Button *button) { this->stop_roomba_button_ = button; };
      void set_pause_roomba_button(button::Button *button) { this->pause_roomba_button_ = button; };
      void set_resume_roomba_button(button::Button *button) { this->resume_roomba_button_ = button; };
      void set_dock_roomba_button(button::Button *button) { this->dock_roomba_button_ = button; };
      void set_find_roomba_button(button::Button *button) { this->find_roomba_button_ = button; };
      void set_evac_roomba_button(button::Button *button) { this->evac_roomba_button_ = button; };
      void set_train_roomba_button(button::Button *button) { this->train_roomba_button_ = button; };

      button::Button *start_roomba_button_{nullptr};
      button::Button *stop_roomba_button_{nullptr};
      button::Button *pause_roomba_button_{nullptr};
      button::Button *resume_roomba_button_{nullptr};
      button::Button *dock_roomba_button_{nullptr};
      button::Button *find_roomba_button_{nullptr};
      button::Button *evac_roomba_button_{nullptr};
      button::Button *train_roomba_button_{nullptr};

      void pause_roomba_action();
      void start_roomba_action();
      void resume_roomba_action();
      void stop_roomba_action();
      void dock_roomba_action();
      void find_roomba_action();
      void evac_roomba_action();
      void train_roomba_action();

      void clean_rooms_by_ids(const std::vector<std::string> &room_ids);
      void clean_room_by_id(const std::string &room_id);

      bool api_call_cmd(const char *command);
      bool api_call(const char *command, JsonObject &additionalArgs);

    protected:
      std::string address_;
      std::string blid_;
      std::string password_;
      struct Region
      {
        std::string region_id;
        std::string region_name;
        std::string region_type;
        std::string type;
      };
      std::string pmap_id_;
      std::string user_pmapv_id_;
      std::vector<Region> regions_;
      esphome::mqtt::MQTTClientComponent *mqtt_client_;

      sensor::Sensor *battery_percent{nullptr};
      sensor::Sensor *rssi_sensor{nullptr};
      text_sensor::TextSensor *cleaning_phase_sensor{nullptr};
      binary_sensor::BinarySensor *bin_full_sensor{nullptr};

    private:
      StaticJsonDocument<1024> wifiStatJsonDoc;
      StaticJsonDocument<16384> stateJsonDoc;
    };

  } // namespace irobot_bridge
} // namespace esphome
