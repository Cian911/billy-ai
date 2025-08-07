#pragma once

#include "esphome/core/component.h"
#include "esphome/components/fan/fan.h"
//#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace billy_mouth_sync {

class BillyMouthSync : public Component {
 public:
  unsigned long last_check_ = 0;
  explicit BillyMouthSync(fan::Fan *motor) : mouth_motor_(motor) {}
  
  void loop() override;
  void start_sync();
  void stop_sync();
  //void set_rms_sensor(sensor::Sensor *sensor) { rms_sensor_ = sensor; }

 protected:
  fan::Fan *mouth_motor_;
  //sensor::Sensor *rms_sensor_{nullptr};
  bool syncing_{false};
};

}  // namespace billy_mouth_sync
}  // namespace esphome

