import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, button
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_BATTERY,
    UNIT_PERCENT,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_MEASUREMENT,
    ICON_BATTERY,
    ICON_SIGNAL,
    UNIT_DECIBEL_MILLIWATT,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    DEVICE_CLASS_RESTART,
    ENTITY_CATEGORY_CONFIG,
    ICON_RESTART_ALERT,
)

CODEOWNERS = []
DEPENDENCIES = []
AUTO_LOAD = ["json", "network", "mqtt", "sensor", "button"]
MULTI_CONF = True

CONF_IROBOT_BRIDGE = "irobot_bridge"
CONF_ADDRESS = "address"
CONF_BLID = "blid"
CONF_PASSWORD = "password"

CONF_BATTERY_PERCENT = "battery_percent"
CONF_RSSI = "rssi"

CONF_START_ROOMBA = "start_roomba"
CONF_STOP_ROOMBA = "stop_roomba"
CONF_PAUSE_ROOMBA = "pause_roomba"
CONF_RESUME_ROOMBA = "resume_roomba"


irobot_bridge_ns = cg.esphome_ns.namespace("irobot_bridge")
IrobotBridge = irobot_bridge_ns.class_(
    "Irobot_Bridge",
    cg.Component,
)

StartRoombaButton = irobot_bridge_ns.class_(
    "StartRoombaButton", button.Button
)
StopRoombaButton = irobot_bridge_ns.class_(
    "StopRoombaButton", button.Button
)
PauseRoombaButton = irobot_bridge_ns.class_(
    "PauseRoombaButton", button.Button
)
ResumeRoombaButton = irobot_bridge_ns.class_(
    "ResumeRoombaButton", button.Button
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
            icon=ICON_BATTERY,
        ),
        cv.Optional(
            CONF_RSSI, default={"id": "rssi", "name": "Roomba Signal Strength"}
        ): sensor.sensor_schema(
            accuracy_decimals=0,
            unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
            state_class=STATE_CLASS_MEASUREMENT,
            icon=ICON_SIGNAL,
        ),
        cv.Optional(
            CONF_START_ROOMBA, default={"id": "start_roomba", "name": "Start Roomba"}
        ): button.button_schema(
            StartRoombaButton,
            device_class=DEVICE_CLASS_RESTART,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_RESTART_ALERT,
        ),
        cv.Optional(
            CONF_STOP_ROOMBA, default={"id": "stop_roomba", "name": "Stop Roomba"}
        ): button.button_schema(
            StopRoombaButton,
            device_class=DEVICE_CLASS_RESTART,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(
            CONF_PAUSE_ROOMBA, default={"id": "pause_roomba", "name": "Pause Roomba"}
        ): button.button_schema(
            PauseRoombaButton,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(
            CONF_RESUME_ROOMBA, default={"id": "resume_roomba", "name": "Resume Roomba"}
        ): button.button_schema(
            ResumeRoombaButton,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    cg.add_define("USE_MQTT")
    cg.add_define("USE_SENSOR")
    cg.add_define("USE_BUTTON")

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

    if start_roomba := config.get(CONF_START_ROOMBA):
        b = await button.new_button(start_roomba)
        await cg.register_parented(b, config[CONF_ID])
        cg.add(var.set_start_roomba_button(b))
    if stop_roomba := config.get(CONF_STOP_ROOMBA):
        b = await button.new_button(stop_roomba)
        await cg.register_parented(b, config[CONF_ID])
        cg.add(var.set_stop_roomba_button(b))
    if pause_roomba := config.get(CONF_PAUSE_ROOMBA):
        b = await button.new_button(pause_roomba)
        await cg.register_parented(b, config[CONF_ID])
        cg.add(var.set_pause_roomba_button(b))
    if resume_roomba := config.get(CONF_RESUME_ROOMBA):
        b = await button.new_button(resume_roomba)
        await cg.register_parented(b, config[CONF_ID])
        cg.add(var.set_resume_roomba_button(b))
