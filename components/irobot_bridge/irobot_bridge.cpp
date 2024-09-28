#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "irobot_bridge.h"
#include "esphome/core/log.h"
#include "esp_log.h"
#include "mbedtls/debug.h"
//#include "ca_cert_chain.h"

// void my_debug(void *ctx, int level, const char *file, int line, const char *str) {
//     ESP_LOGI(TAG, "%s:%04d: %s", file, line, str);
// }

namespace esphome
{
  namespace irobot_bridge
  {

    static const char *const TAG = "Irobot_Bridge";

    float Irobot_Bridge::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }

    void Irobot_Bridge::setup()
    {
      ESP_LOGI(TAG, "Irobot_Bridge calling setup()");
  //        mbedtls_debug_set_threshold(4);  // Set the debug level (0 = no debug, 4 = max verbosity)
    //mbedtls_ssl_conf_dbg(&your_tls_config, my_debug, NULL);
      this->mqtt_client_ = new esphome::mqtt::MQTTClientComponent();

      this->mqtt_client_->set_broker_address(this->address_);
      this->mqtt_client_->set_username(this->blid_);
      this->mqtt_client_->set_password(this->password_);
      this->mqtt_client_->set_broker_port(8883);
      this->mqtt_client_->set_keep_alive(60);
      this->mqtt_client_->set_client_id(this->blid_);
      this->mqtt_client_->set_skip_cert_cn_check(true);
      this->mqtt_client_->disable_discovery();

      this->mqtt_client_->set_ca_certificate((const char*)ca_cert_chain_pem);

      // esphome::mqtt::MQTTMessage last_will_message;
      // last_will_message.topic = "irobot/LWT";
      // last_will_message.payload = "offline";
      // last_will_message.qos = 0;
      // last_will_message.retain = true;
      // this->mqtt_client_->set_last_will(std::move(last_will_message));

      this->mqtt_client_->set_on_connect([this](bool sessionPresent)
                                         { this->onMqttConnect(sessionPresent); });

      this->mqtt_client_->set_on_disconnect([this](esphome::mqtt::MQTTClientDisconnectReason reason)
                                            { this->onMqttDisconnect(reason); });
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
     // if (this->mqtt_client_->is_connected())
      //{
       // ESP_LOGI(TAG, "connected to mqtt on [%s], waiting for something to do", to_string(this->address_).c_str());
        //  //  } else {
        //     //this->connect();
      //}
      // ESP_LOGI(TAG, "currently in mqtt state %d", this->mqtt_client_->get_component_state());

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

    void Irobot_Bridge::update()
    {
      // this->is_update_requested_ = true;
    }

    // void Irobot_Bridge::report_error(esp32_ble_tracker::ClientState state) {
    //    this->state_ = state;
    //    this->status_set_warning();
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

  } // namespace myhomeiot_ble_client
} // namespace esphome
