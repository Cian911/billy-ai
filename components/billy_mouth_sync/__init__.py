print("DEBUG: billy_mouth_sync component loaded")

import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID
from esphome.components import fan

billy_mouth_sync_ns = cg.esphome_ns.namespace("billy_mouth_sync")
BillyMouthSync = billy_mouth_sync_ns.class_("BillyMouthSync", cg.Component)

CONF_MOUTH_MOTOR_ID = "mouth_motor_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BillyMouthSync),
    cv.Required(CONF_MOUTH_MOTOR_ID): cv.use_id(fan.Fan)
})

async def to_code(config):
    motor = await cg.get_variable(config[CONF_MOUTH_MOTOR_ID])
    var = cg.new_Pvariable(config[CONF_ID], motor)
    await cg.register_component(var, config)
