#pragma once
#include "esphome/components/speaker/speaker.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include <cstdint>

namespace esphome {
namespace speaker_tap {

class TapSpeaker : public speaker::Speaker, public Component {
 public:
  void set_sink(speaker::Speaker *s) { sink_ = s; }
  void set_level_sensor(sensor::Sensor *s) { level_sensor_ = s; }
  void set_attack_ms(uint32_t v) { attack_ms_ = v; }
  void set_release_ms(uint32_t v) { release_ms_ = v; }
  void set_publish_interval(uint32_t ms) { publish_interval_ms_ = ms; }
  void set_gain(float g) { gain_ = g; }

  void setup() override {}
  float get_setup_priority() const override { return setup_priority::AFTER_CONNECTION; }

  // Speaker overrides (pass through)
  void start() override { if (sink_) sink_->start(); }
  void stop() override { if (sink_) sink_->stop(); }
  void set_volume(float v) override { if (sink_) sink_->set_volume(v); }
  void set_mute_state(bool m) override { if (sink_) sink_->set_mute_state(m); }
  void set_pause_state(bool p) override { if (sink_) sink_->set_pause_state(p); }
  bool has_buffered_data() const override { return sink_ ? sink_->has_buffered_data() : false; }

  void set_audio_stream_info(const audio::AudioStreamInfo &info) override {
    stream_info_ = info;
    if (sink_) sink_->set_audio_stream_info(info);
  }

  size_t play(const uint8_t *data, size_t length) override;

 protected:
  speaker::Speaker *sink_{nullptr};
  sensor::Sensor *level_sensor_{nullptr};
  audio::AudioStreamInfo stream_info_{};
  float env_ = 0.0f;
  uint32_t publish_interval_ms_{40};
  uint32_t last_pub_ms_{0};
  uint32_t attack_ms_{8};
  uint32_t release_ms_{120};
  float gain_{1.0f};
};

}  // namespace speaker_tap
}  // namespace esphome
