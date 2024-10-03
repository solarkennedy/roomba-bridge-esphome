import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, mqtt
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_BATTERY,
    UNIT_PERCENT,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_MEASUREMENT,
    UNIT_DECIBEL_MILLIWATT,
    DEVICE_CLASS_SIGNAL_STRENGTH,
)

CODEOWNERS = []
DEPENDENCIES = []
AUTO_LOAD = ["json", "network", "mqtt", "sensor"]
MULTI_CONF = True

CONF_IROBOT_BRIDGE = "irobot_bridge"
CONF_ADDRESS = "address"
CONF_BLID = "blid"
CONF_PASSWORD = "password"

CONF_BATTERY_PERCENT = "battery_percent"
CONF_RSSI = "rssi"

irobot_bridge_ns = cg.esphome_ns.namespace("irobot_bridge")
IrobotBridge = irobot_bridge_ns.class_(
    "Irobot_Bridge",
    cg.Component,
)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(IrobotBridge),
        cv.Required(CONF_ADDRESS): cv.string,
        cv.Required(CONF_BLID): cv.string,
        cv.Required(CONF_PASSWORD): cv.string,
        cv.Optional(
            CONF_BATTERY_PERCENT,
            default={"id": "battery_percent", "name": "Battery Percent"},
        ): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            device_class=DEVICE_CLASS_BATTERY,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(
            CONF_RSSI, default={"id": "rssi", "name": "Roomba Signal Strength"}
        ): sensor.sensor_schema(
            accuracy_decimals=0,
            unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    cg.add_define("USE_MQTT")
    cg.add_define("USE_SENSOR")

    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_blid(config[CONF_BLID]))
    cg.add(var.set_password(config[CONF_PASSWORD]))
    await cg.register_component(var, config)
  
    if rssi_config := config.get(CONF_RSSI):
        sens = await sensor.new_sensor(rssi_config)
        cg.add(var.set_rssi_sensor(sens))

    if battery_percent_config := config.get(CONF_BATTERY_PERCENT):
        sens = await sensor.new_sensor(battery_percent_config)
        cg.add(var.set_battery_percent_sensor(sens))