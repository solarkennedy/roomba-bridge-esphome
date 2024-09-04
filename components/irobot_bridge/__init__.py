import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, mqtt
from esphome.const import CONF_ID, UNIT_EMPTY, ICON_EMPTY

CODEOWNERS = []
DEPENDENCIES = []
AUTO_LOAD = ["json", "network", "mqtt"]
MULTI_CONF = True

CONF_IROBOT_BRIDGE = "irobot_bridge"
CONF_ADDRESS = "address"
CONF_BLID = "blid"
CONF_PASSWORD = "password"

irobot_bridge_ns = cg.esphome_ns.namespace('irobot_bridge')
IrobotBridge = irobot_bridge_ns.class_('Irobot_Bridge', cg.PollingComponent)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(IrobotBridge),
            cv.Required(CONF_ADDRESS): cv.string,
            cv.Required(CONF_BLID): cv.string,
            cv.Required(CONF_PASSWORD): cv.string,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):
    cg.add_define("USE_MQTT")
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)