#include "tap_speaker.h"
#include "esphome/core/helpers.h"  // for esphome::millis
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace esphome {
namespace speaker_tap {

// Helper to compute block time constants
static inline float exp_tc(float ms, float tc_ms) {
  if (tc_ms <= 1.0f) return 0.0f;
  return std::exp(-ms / tc_ms);
}

size_t TapSpeaker::play(const uint8_t *data, size_t length) {
  // Assume signed 16-bit PCM (WAV pipeline), mono or stereo
  const size_t n_s16 = length / sizeof(int16_t);
  const int16_t *s16 = reinterpret_cast<const int16_t *>(data);

  double sumsq = 0.0;
  for (size_t i = 0; i < n_s16; i++) {
    float v = std::abs((int)s16[i]) / 32768.0f;  // 0..1
    sumsq += double(v) * double(v);
  }

  if (n_s16 > 0) {
    // Get stream info via getters (new API)
    const uint8_t  bps = this->stream_info_.get_bits_per_sample();
    const uint8_t  ch  = this->stream_info_.get_channels();
    const uint32_t sr  = this->stream_info_.get_sample_rate();

    // Derive approximate block duration
    float block_ms = (1000.0f * float(n_s16)) / float(sr * ch);
    float rms = std::sqrt(sumsq / double(n_s16));  // 0..1

    // Attack/release smoothing
    float att = exp_tc(block_ms, float(attack_ms_));
    float rel = exp_tc(block_ms, float(release_ms_));
    if (rms > env_) env_ = rms + (env_ - rms) * att;
    else            env_ = rms + (env_ - rms) * rel;

    // Publish every publish_interval_ms_ (via sample counting)
    samples_since_pub_ += n_s16;
    const uint64_t publish_every_samples =
        (uint64_t(sample_rate_) * uint64_t(publish_interval_ms_)) / 1000ULL;

    if (level_sensor_ && publish_every_samples > 0 &&
        samples_since_pub_ >= publish_every_samples) {
      float level = std::clamp(env_ * gain_, 0.0f, 1.0f);
      level_sensor_->publish_state(level * 100.0f);  // percent
      samples_since_pub_ = 0;
    }

    // Example of timing (if you need wall time for failsafes)
    uint32_t now = esphome::millis();
    (void) now;  // avoid unused warning if not used
  }

  return sink_ ? sink_->play(data, length) : length;
}

}  // namespace speaker_tap
}  // namespace esphome

