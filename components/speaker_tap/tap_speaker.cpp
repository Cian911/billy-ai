#include "tap_speaker.h"
#include "esphome/core/log.h"
#include <algorithm>
#include <cmath>

namespace esphome {
namespace speaker_tap {

static const char *const TAG = "speaker_tap";

size_t TapSpeaker::play(const uint8_t *data, size_t length) {
  const uint8_t bps = this->stream_info_.bits_per_sample;  // 16 or 32
  const uint8_t ch  = this->stream_info_.channels;         // 1 or 2
  const uint32_t sr = this->stream_info_.sample_rate;
  if (bps == 0 || ch == 0 || sr == 0) {
    return sink_ ? sink_->play(data, length) : length;
  }

  const size_t bytes_per_sample = bps / 8;
  const size_t frame_size = bytes_per_sample * ch;
  const size_t n_frames = length / frame_size;

  // Accumulate normalized energy
  double sumsq = 0.0;
  for (size_t i = 0; i < n_frames; i++) {
    float mixed = 0.0f;
    const uint8_t *p = data + i * frame_size;
    for (uint8_t c = 0; c < ch; c++) {
      if (bps == 16) {
        int16_t s = reinterpret_cast<const int16_t *>(p)[c];
        mixed += std::abs((int)s) / 32768.0f;
      } else {  // 32-bit
        int32_t s = reinterpret_cast<const int32_t *>(p)[c];
        mixed += (std::abs((int64_t)s) / 2147483648.0f);
      }
    }
    mixed /= ch;
    sumsq += double(mixed) * double(mixed);
  }

  if (n_frames > 0) {
    float rms = std::sqrt(sumsq / double(n_frames));  // 0..1
    // Per-block attack/release smoothing
    const float block_ms = 1000.0f * float(n_frames) / float(sr);
    const float att = std::exp(-block_ms / float(std::max<uint32_t>(1, attack_ms_)));
    const float rel = std::exp(-block_ms / float(std::max<uint32_t>(1, release_ms_)));
    if (rms > env_) env_ = rms + (env_ - rms) * att;
    else            env_ = rms + (env_ - rms) * rel;

    float level = std::clamp(env_ * gain_, 0.0f, 1.0f);
    uint32_t now = millis();
    if (level_sensor_ && now - last_pub_ms_ >= publish_interval_ms_) {
      level_sensor_->publish_state(level * 100.0f);  // percent
      last_pub_ms_ = now;
    }
  }

  return sink_ ? sink_->play(data, length) : length;
}

}  // namespace speaker_tap
}  // namespace esphome
