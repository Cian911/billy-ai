#include "billy_mouth_sync.h"
#include "esphome/core/log.h"

namespace esphome {
namespace billy_mouth_sync {

static const char *const TAG = "BillyMouthSync";

void BillyMouthSync::start_sync() {
  ESP_LOGI(TAG, "Starting mouth sync");
  syncing_ = true;
}

void BillyMouthSync::stop_sync() {
  ESP_LOGI(TAG, "Stopping mouth sync");
  syncing_ = false;
  if (mouth_motor_ != nullptr)
    mouth_motor_->turn_off().perform();
}

void BillyMouthSync::loop() {
  if (!syncing_ || mouth_motor_ == nullptr)
    return;

  //float amplitude = rms_sensor_->state;
  //ESP_LOGD(TAG, "Amplitude: %.2f", amplitude);

  if (millis() - last_check_ > 300) { // toggle every 0.3 sec
    last_check_ = millis();
    static bool mouth_open_ = false;
    mouth_open_ = !mouth_open_;
    if (mouth_open_) {
      mouth_motor_->turn_on().perform();
    } else {
      mouth_motor_->turn_off().perform();
    }
  }
}

}  // namespace billy_mouth_sync
}  // namespace esphome

