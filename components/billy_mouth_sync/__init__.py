import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import fan, sensor
from esphome.const import CONF_ID

CONF_MOUTH_MOTOR_ID = "mouth_motor_id"
#CONF_AUDIO_RMS_ID = "audio_rms_id"

billy_mouth_sync_ns = cg.esphome_ns.namespace("billy_mouth_sync")
BillyMouthSync = billy_mouth_sync_ns.class_("BillyMouthSync", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BillyMouthSync),
    cv.Required(CONF_MOUTH_MOTOR_ID): cv.use_id(fan.Fan),
    #cv.Required(CONF_AUDIO_RMS_ID): cv.use_id(sensor.Sensor),
})

async def to_code(config):
    motor = await cg.get_variable(config[CONF_MOUTH_MOTOR_ID])
    var = cg.new_Pvariable(config[CONF_ID], motor)
    await cg.register_component(var, config)
    
    #sensor_ = await cg.get_variable(config[CONF_AUDIO_RMS_ID])
    #cg.add(var.set_rms_sensor(sensor_))

