#include "billy_mouth_sync.h"

namespace esphome {
namespace billy_mouth_sync {

BillyMouthSync::BillyMouthSync(fan::Fan *motor) {
  mouth_motor = motor;
}

void BillyMouthSync::start_sync() {
  ESP_LOGI("BillyMouthSync", "Starting mouth sync");
  syncing = true;
}

void BillyMouthSync::stop_sync() {
  ESP_LOGI("BillyMouthSync", "Stopping mouth sync");
  syncing = false;
  if (mouth_motor) mouth_motor->turn_off();
}

void BillyMouthSync::loop() {
  if (!syncing || !mouth_motor) return;

  if (millis() - last_check > 300) { // toggle every 0.3 sec
    last_check = millis();
    static bool mouth_open = false;
    mouth_open = !mouth_open;
    if (mouth_open) {
      mouth_motor->turn_on();
    } else {
      mouth_motor->turn_off();
    }
  }
}

}  // namespace billy_mouth_sync
}  // namespace esphome
