#pragma once
#include "esphome.h"

namespace esphome {
namespace billy_mouth_sync {

class BillyMouthSync : public Component {
 public:
  fan::Fan *mouth_motor;
  bool syncing = false;
  unsigned long last_check = 0;

  BillyMouthSync(fan::Fan *motor);
  void start_sync();
  void stop_sync();
  void loop() override;
};

}  // namespace billy_mouth_sync
}  // namespace esphome
