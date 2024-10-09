#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "roomba_bridge.h"
#include "esphome/core/log.h"
#include "esp_log.h"
#include "esphome/components/json/json_util.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
  namespace roomba_bridge
  {

    static const char *const TAG = "Roomba_Bridge";

    float Roomba_Bridge::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }

    void Roomba_Bridge::setup()
    {
      ESP_LOGI(TAG, "Roomba_Bridge calling setup()");
      this->mqtt_client_ = new esphome::mqtt::MQTTClientComponent();

      this->mqtt_client_->set_broker_address(this->address_);
      this->mqtt_client_->set_username(this->blid_);
      this->mqtt_client_->set_password(this->password_);
      this->mqtt_client_->set_broker_port(8883);
      this->mqtt_client_->set_keep_alive(60);
      this->mqtt_client_->set_client_id(this->blid_);
      this->mqtt_client_->set_skip_cert_cn_check(true);
      this->mqtt_client_->disable_discovery();
      this->mqtt_client_->disable_log_message();
      this->mqtt_client_->set_ca_certificate((const char *)ca_cert_chain_pem);

      this->mqtt_client_->set_on_connect([this](bool sessionPresent)
                                         { this->onMqttConnect(sessionPresent); });

      this->mqtt_client_->set_on_disconnect([this](esphome::mqtt::MQTTClientDisconnectReason reason)
                                            { this->onMqttDisconnect(reason); });

      // Create update string that looks like this:
      // $aws/things/FD74AF5E78124D5CA47C693F19F966B3/shadow/update
      std::string update_string = "$aws/things/" + this->blid_ + "/shadow/update";
      this->mqtt_client_->subscribe(update_string, [this](const std::string &topic, const std::string &message)
                                    { this->handle_update_message(topic, message); }, 0);

      std::string wifistat_topic = "wifistat";
      this->mqtt_client_->subscribe(wifistat_topic, [this](const std::string &topic, const std::string &message)
                                    { this->handle_wifistat_message(topic, message); }, 0);

      this->mqtt_client_->setup();
      ESP_LOGI(TAG, "Roomba_Bridge setup() done");
    }

    void Roomba_Bridge::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Roomba Bridge:");
      ESP_LOGCONFIG(TAG, "  IP Address: %s", this->address_.c_str());
      ESP_LOGCONFIG(TAG, "  BLID: %s", this->blid_.c_str());
      ESP_LOGCONFIG(TAG, "  Password: %s", this->password_.c_str());
      if (!pmap_id_.empty())
      {
        ESP_LOGCONFIG(TAG, "  pmap_id: %s", pmap_id_.c_str());
      }
      if (!user_pmapv_id_.empty())
      {
        ESP_LOGCONFIG(TAG, "  user_pmapv_id: %s", user_pmapv_id_.c_str());
      }
      if (!regions_.empty())
      {
        ESP_LOGCONFIG(TAG, "  regions:");
        for (auto &region : regions_)
        {
          ESP_LOGCONFIG(TAG, "    - region_id: %s", region.region_id.c_str());
          ESP_LOGCONFIG(TAG, "      region_name: %s", region.region_name.c_str());
          ESP_LOGCONFIG(TAG, "      region_type: %s", region.region_type.c_str());
          ESP_LOGCONFIG(TAG, "      type: %s", region.type.c_str());
        }
      }
    }

    void Roomba_Bridge::loop()
    {
      this->mqtt_client_->loop();
      return;
    }

    void Roomba_Bridge::connect()
    {
      ESP_LOGI(TAG, "[%s] Connecting", to_string(this->address_).c_str());
      this->mqtt_client_->setup();
      return;
    }

    void Roomba_Bridge::disconnect()
    {
      ESP_LOGI(TAG, "[%s] Disconnecting", to_string(this->address_).c_str());
      this->mqtt_client_->on_shutdown();
      return;
    }

    void Roomba_Bridge::handle_update_message(const std::string &topic, const std::string &doc)
    {
      stateJsonDoc.clear();
      DeserializationError error = deserializeJson(stateJsonDoc, doc);
      if (error)
      {
        ESP_LOGE(TAG, "Error parsing update JSON: %s", error.c_str());
        return;
      }

      if (!stateJsonDoc.containsKey("state") && stateJsonDoc["state"].containsKey("reported"))
      {
        ESP_LOGW(TAG, "Got unexpected state message, doesn't have state.reported??? %s", doc.c_str());
        return;
      }

      if (stateJsonDoc["state"]["reported"].containsKey("batPct") && this->battery_percent != nullptr)
      {
        int batPct = stateJsonDoc["state"]["reported"]["batPct"];
        this->battery_percent->publish_state(batPct);
      }

      const char *phase = stateJsonDoc["state"]["reported"]["cleanMissionStatus"]["phase"];
      if (phase != nullptr && this->cleaning_phase_sensor != nullptr)
      {
        this->cleaning_phase_sensor->publish_state(phase);
      }

      bool binPresent = stateJsonDoc["state"]["reported"]["bin"]["present"];
      bool binFull = stateJsonDoc["state"]["reported"]["bin"]["full"];
      if (binPresent && this->bin_full_sensor != nullptr)
      {
        this->bin_full_sensor->publish_state(binFull);
      }
    }

    /*
     {"state":{"reported":{"signal":{"rssi":-33,"snr":63,"noise":-96}}}}
    */
    void Roomba_Bridge::handle_wifistat_message(const std::string &topic, const std::string &doc)
    {

      wifiStatJsonDoc.clear();
      DeserializationError error = deserializeJson(wifiStatJsonDoc, doc);
      if (error)
      {
        ESP_LOGE(TAG, "Error parsing wifistat JSON: %s", error.c_str());
        return;
      }

      if (!wifiStatJsonDoc.containsKey("state") && wifiStatJsonDoc["state"].containsKey("reported"))
      {
        ESP_LOGW(TAG, "Got unexpected wifistat message, doesn't have state.reported??? %s", doc.c_str());
        return;
      }
      int rssi_value = wifiStatJsonDoc["state"]["reported"]["signal"]["rssi"];
      if (this->rssi_sensor != nullptr)
      {
        this->rssi_sensor->publish_state(rssi_value);
      }
    }

    void Roomba_Bridge::start_roomba_action() { api_call_cmd("start"); };
    void Roomba_Bridge::stop_roomba_action() { api_call_cmd("stop"); };
    void Roomba_Bridge::pause_roomba_action() { api_call_cmd("pause"); };
    void Roomba_Bridge::resume_roomba_action() { api_call_cmd("resume"); };
    void Roomba_Bridge::dock_roomba_action() { api_call_cmd("dock"); };
    void Roomba_Bridge::evac_roomba_action() { api_call_cmd("evac"); };
    void Roomba_Bridge::find_roomba_action() { api_call_cmd("find"); };
    void Roomba_Bridge::train_roomba_action() { api_call_cmd("train"); };

    bool Roomba_Bridge::api_call_cmd(const char *cmd)
    {
      StaticJsonDocument<1> doc;
      JsonObject null_obj = doc.to<JsonObject>();
      return this->api_call(cmd, null_obj);
    }

    bool Roomba_Bridge::api_call(const char *command, JsonObject &additionalArgs)
    {
      StaticJsonDocument<200> doc;
      doc["command"] = command;
      doc["time"] = millis() / 1000;
      doc["initiator"] = "localApp";

      if (!additionalArgs.isNull())
      {
        for (JsonPair p : additionalArgs)
        {
          doc[p.key()] = p.value();
        }
      }

      char json_buffer[256];
      serializeJson(doc, json_buffer);
      return this->mqtt_client_->publish("cmd", json_buffer, strlen(json_buffer), 0, false);
    }

    void Roomba_Bridge::clean_rooms_by_ids(const std::vector<std::string> &room_ids)
    {
      DynamicJsonDocument doc(2048);
      JsonObject args = doc.to<JsonObject>(); 
      args["ordered"] = 1;
      args["pmap_id"] = this->pmap_id_;
      args["user_pmapv_id"] = this->user_pmapv_id_;
      JsonArray regions = args.createNestedArray("regions");
      for (const auto &room_id : room_ids)
      {
        for (const auto &region : this->regions_)
        {
          if (region.region_id == room_id)
          {
            JsonObject region_obj = regions.createNestedObject();
            region_obj["region_id"] = region.region_id;
            region_obj["region_name"] = region.region_name;
            region_obj["region_type"] = region.region_type;
            region_obj["type"] = region.type;
            break;
          }
        }
      }
      this->api_call("clean", args);
    }

    // Function to clean a single room by its ID
    void Roomba_Bridge::clean_room_by_id(const std::string &room_id)
    {
      this->clean_rooms_by_ids({room_id});
    }

    void Roomba_Bridge::onMqttConnect(bool session)
    {
      ESP_LOGI(TAG, "Connected to roomba MQTT.");
    }

    void Roomba_Bridge::onMqttDisconnect(esphome::mqtt::MQTTClientDisconnectReason reason)
    {
      switch (reason)
      {
      case esphome::mqtt::MQTTClientDisconnectReason::TCP_DISCONNECTED:
        ESP_LOGE(TAG, "Disconnected from roomba MQTT. Reason: TCP Disconnected");
        break;
      case esphome::mqtt::MQTTClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
        ESP_LOGE(TAG, "Disconnected from roomba MQTT. Reason: Unacceptable protocol version");
        break;
      case esphome::mqtt::MQTTClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
        ESP_LOGE(TAG, "Disconnected from roomba MQTT. Reason: Identifier rejected");
        break;
      case esphome::mqtt::MQTTClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
        ESP_LOGE(TAG, "Disconnected from roomba MQTT. Reason: Server unavailable");
        break;
      case esphome::mqtt::MQTTClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
        ESP_LOGE(TAG, "Disconnected from roomba MQTT. Reason: Malformed credentials");
        break;
      case esphome::mqtt::MQTTClientDisconnectReason::MQTT_NOT_AUTHORIZED:
        ESP_LOGE(TAG, "Disconnected from roomba MQTT. Reason: Not authorized");
        break;
      default:
        ESP_LOGE(TAG, "Disconnected from roomba MQTT. Unknown reason");
      }
    }

  } // namespace roomba_bridge
} // namespace esphome
