#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "irobot_bridge.h"
#include "esphome/core/log.h"
#include "esp_log.h"
#include "esphome/components/json/json_util.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome
{
  namespace irobot_bridge
  {

    static const char *const TAG = "Irobot_Bridge";

    float Irobot_Bridge::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }

    void Irobot_Bridge::setup()
    {
      ESP_LOGI(TAG, "Irobot_Bridge calling setup()");
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
      ESP_LOGI(TAG, "Irobot_Bridge setup() done");
    }

    void Irobot_Bridge::dump_config()
    {
      ESP_LOGCONFIG(TAG, "iRobot Bridge:");
      ESP_LOGCONFIG(TAG, "  IP Address: %s", this->address_.c_str());
      ESP_LOGCONFIG(TAG, "  BLID: %s", this->blid_.c_str());
      ESP_LOGCONFIG(TAG, "  Password: %s", this->password_.c_str());
    }

    void Irobot_Bridge::loop()
    {
      this->mqtt_client_->loop();
      return;
    }

    void Irobot_Bridge::connect()
    {
      ESP_LOGI(TAG, "[%s] Connecting", to_string(this->address_).c_str());
      this->mqtt_client_->setup();
      return;
    }

    void Irobot_Bridge::disconnect()
    {
      ESP_LOGI(TAG, "[%s] Disconnecting", to_string(this->address_).c_str());
      this->mqtt_client_->on_shutdown();
      return;
    }

    void Irobot_Bridge::handle_update_message(const std::string &topic, const std::string &doc)
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
    void Irobot_Bridge::handle_wifistat_message(const std::string &topic, const std::string &doc)
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

    void Irobot_Bridge::start_roomba_action() { api_call_cmd("start"); };
    void Irobot_Bridge::stop_roomba_action() { api_call_cmd("stop"); };
    void Irobot_Bridge::pause_roomba_action() { api_call_cmd("pause"); };
    void Irobot_Bridge::resume_roomba_action() { api_call_cmd("resume"); };
    void Irobot_Bridge::dock_roomba_action() { api_call_cmd("dock"); };
    void Irobot_Bridge::evac_roomba_action() { api_call_cmd("evac"); };
    void Irobot_Bridge::find_roomba_action() { api_call_cmd("find"); };
    void Irobot_Bridge::train_roomba_action() { api_call_cmd("train"); };

    bool Irobot_Bridge::api_call_cmd(const char *cmd)
    {
      StaticJsonDocument<1> doc;
      JsonObject null_obj = doc.to<JsonObject>();
      return this->api_call(cmd, null_obj);
    }

    bool Irobot_Bridge::api_call(const char *command, JsonObject &additionalArgs)
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

    void Irobot_Bridge::onMqttConnect(bool session)
    {
      ESP_LOGI(TAG, "Connected to roomba MQTT.");
    }

    void Irobot_Bridge::onMqttDisconnect(esphome::mqtt::MQTTClientDisconnectReason reason)
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

  } // namespace irobot_bridge
} // namespace esphome
