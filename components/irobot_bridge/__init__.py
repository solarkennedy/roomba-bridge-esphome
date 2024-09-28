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

async def to_code(config):
    cg.add_define("USE_MQTT")
    # cg.add_define("ASYNC_TCP_SSL_ENABLED", True)
    # cg.add_define("SHA1_SIZE", 20)
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_blid(config[CONF_BLID]))
    cg.add(var.set_password(config[CONF_PASSWORD]))
    await cg.register_component(var, config)