# 🎤 billy-ai

`billy-ai` is an [ESPHome](https://esphome.io) **external component** that lets you _tap into_ an I²S speaker stream and measure real-time audio levels.  
This is especially useful for animatronics (like a Billy Bass 🐟) where you want to synchronize a motorized mouth or body part with the audio being played.

---

## ✨ Features

- Taps into the existing ESPHome `speaker` pipeline.
- Extracts RMS (root mean square) audio energy from I²S audio output.
- Publishes a smoothed percentage level (`0–100%`) to a sensor.
- Configurable **gain**, **attack/release smoothing**, **publish interval**, and **sample rate**.
- Works alongside your existing `i2s_audio` speaker setup.

---

## 📦 Installation

Add `billy-ai` as an external component in your ESPHome YAML:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/Cian911/billy-ai
      ref: main
      components: [speaker_tap]
```

---

## ⚡ Usage Example

Here’s a minimal setup that plays audio via I²S and taps the stream to measure its level:

```yaml
speaker:
  - platform: i2s_audio
    id: spk_hw
    dac_type: external
    i2s_audio_id: i2s_out
    i2s_dout_pin: GPIO21
    # mode: mono  # optional

  - platform: speaker_tap
    id: spk_tap
    sink: spk_hw
    gain: 6.0
    attack_ms: 8
    release_ms: 120
    publish_interval: 80ms
    sample_rate: 22050 # must match your audio pipeline
    level:
      id: audio_level
      name: "Audio Level"
```

---

## 🐟 Animatronic Mouth Example

Here’s how you might drive a servo or motor to move in sync with the audio:

```yaml
  - platform: speaker_tap
    ...
    level:
      id: audio_level
      name: "Audio Level"
      on_value:
        then:
          - lambda: |-
              static float ema = 0.0f;
              const float alpha = 0.25f;        // smoothing factor
              float v = x;                      // x = % level
              ema = (1.0f - alpha) * ema + alpha * v;

              const float OPEN_TH   = 6.0f;     // open threshold
              const float CLOSE_TH  = 6.0f;     // close threshold
              const uint32_t REFRESH_MS = 140;  // nudge while loud
              const uint32_t MIN_GAP_MS = 40;   // debounce
              const uint32_t FAILSAFE_MS = 900; // failsafe auto-close

              static bool open = false;
              static uint32_t last_ms = 0;
              uint32_t now = millis();

              if (!open) {
                if (ema > OPEN_TH && (now - last_ms > MIN_GAP_MS)) {
                  id(mouth_open_pulse).execute();
                  open = true;
                  last_ms = now;
                }
              } else {
                if (ema > OPEN_TH) {
                  if (now - last_ms > REFRESH_MS) {
                    id(mouth_open_keep).execute();
                    last_ms = now;
                  }
                } else if (ema < CLOSE_TH) {
                  id(mouth_close_pulse).execute();
                  open = false;
                  last_ms = now;
                }
                if (open && (now - last_ms > FAILSAFE_MS)) {
                  id(mouth_close_pulse).execute();
                  open = false;
                  last_ms = now;
                }
              }
```

---

## ⚙️ Configuration Options

| Option             | Type   | Default | Description                                         |
| ------------------ | ------ | ------- | --------------------------------------------------- |
| `sink`             | ID     | –       | The actual speaker component to tap.                |
| `gain`             | float  | `1.0`   | Amplifies or attenuates the detected level.         |
| `attack_ms`        | int    | `8`     | Attack smoothing time (ms).                         |
| `release_ms`       | int    | `120`   | Release smoothing time (ms).                        |
| `publish_interval` | time   | `80ms`  | How often to publish the level sensor.              |
| `sample_rate`      | int    | `22050` | Must match the I²S audio pipeline sample rate.      |
| `level`            | sensor | –       | Sensor where the audio level (0–100%) is published. |
