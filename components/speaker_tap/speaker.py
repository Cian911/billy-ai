import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import speaker, sensor
from esphome.const import CONF_ID, UNIT_PERCENT, STATE_CLASS_MEASUREMENT  # <- no ICON_WAVEFORM

speaker_tap_ns = cg.esphome_ns.namespace("speaker_tap")
TapSpeaker = speaker_tap_ns.class_("TapSpeaker", speaker.Speaker, cg.Component)

CONF_SINK = "sink"
CONF_LEVEL = "level"
CONF_ATTACK_MS = "attack_ms"
CONF_RELEASE_MS = "release_ms"
CONF_PUBLISH_INTERVAL = "publish_interval"
CONF_GAIN = "gain"

LEVEL_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_PERCENT,
    icon="mdi:waveform",                 # <- use a literal mdi icon string
    accuracy_decimals=0,
    state_class=STATE_CLASS_MEASUREMENT,
)

CONFIG_SCHEMA = speaker.SPEAKER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(TapSpeaker),
        cv.Required(CONF_SINK): cv.use_id(speaker.Speaker),
        cv.Optional(CONF_LEVEL): LEVEL_SCHEMA,
        cv.Optional(CONF_ATTACK_MS, default=8): cv.positive_int,
        cv.Optional(CONF_RELEASE_MS, default=120): cv.positive_int,
        cv.Optional(CONF_PUBLISH_INTERVAL, default="40ms"): cv.update_interval,
        cv.Optional(CONF_GAIN, default=1.0): cv.float_,
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await speaker.register_speaker(var, config)

    sink = await cg.get_variable(config[CONF_SINK])
    cg.add(var.set_sink(sink))
    cg.add(var.set_attack_ms(config[CONF_ATTACK_MS]))
    cg.add(var.set_release_ms(config[CONF_RELEASE_MS]))
    cg.add(var.set_publish_interval(config[CONF_PUBLISH_INTERVAL]))
    cg.add(var.set_gain(config[CONF_GAIN]))

    if CONF_LEVEL in config:
        sens = await sensor.new_sensor(config[CONF_LEVEL])
        cg.add(var.set_level_sensor(sens))

