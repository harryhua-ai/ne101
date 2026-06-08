# Cloud Onboarding JSON Reference

Topic: `camera/{mac}/default` — payload is a JSON object; all fields are optional.

Numeric fields accept JSON number, string, or boolean (`true`/`false` → 1/0). Out-of-range values are clamped to the allowed range; invalid `country_code` and `frameSize` values are rejected (current value kept).

---

## MQTT

| Field | Alias | Values |
|-------|-------|--------|
| `mqtt_host` | `host` | string, max 127 chars |
| `mqtt_port` | `port` | 1–65535 |
| `topic` | — | string, max 127 chars |
| `client_id` | `clientId` | string, max 127 chars |
| `mqtt_user` | `user` | string, max 127 chars |
| `mqtt_password` | `password` | string, max 127 chars |
| `mqtt_qos` | `qos` | 0, 1, 2 |
| `mqtt_tls_enable` | `tlsEnable` | 0, 1 |

---

## Image

| Field | Alias | Values |
|-------|-------|--------|
| `resolution` | — | `320x240`, `640x480`, `800x600`, `1024x768`, `1280x720`, `1280x1024`, `1600x1200`, `1920x1080`, `2048x1536`, `2560x1920` |
| `frameSize` | — | 5, 8, 9, 10, 11, 12, 13, 14, 17, 21 (same as resolution table) |
| `image_quality` | `quality` | 0–63 (higher = lower quality) |
| `brightness` | — | -2 to 2 |
| `contrast` | — | -2 to 2 |
| `saturation` | — | -2 to 2 |
| `aeLevel` | — | -2 to 2 |
| `bAgc` | — | 0, 1 |
| `gain` | — | 0–30 or 64 |
| `gainCeiling` | — | 0–6 |
| `bHorizonetal` | `horizontal_mirror` | 0, 1 |
| `bVertical` | `vertical_flip` | 0, 1 |
| `hdr_enable` | `hdrEnable` | 0, 1 |

---

## Fill Light

| Field | Alias | Values |
|-------|-------|--------|
| `light_mode` | `lightMode` | `auto`/`0`, `customize`/`custom`/`1`, `on`/`2`, `off`/`3` |
| `light_threshold` | `threshold` | 0–100 |
| `light_brightness` | `duty` | 0–100 |
| `light_start_time` | `startTime` | `HH:MM` |
| `light_end_time` | `endTime` | `HH:MM` |

---

## Capture

| Field | Alias | Values |
|-------|-------|--------|
| `capture_mode` | — | `off`/`disabled`, `interval`, `timed` (or other → timed mode) |
| `capture_interval` | — | `"<n> min"`, `"<n> hour"`, `"<n> day"` |
| `bScheCap` | — | 0, 1 |
| `bAlarmInCap` | — | 0, 1 |
| `bButtonCap` | — | 0, 1 |
| `scheCapMode` | — | 0 = timed, 1 = interval |
| `intervalValue` | — | ≥ 1 |
| `intervalUnit` | — | 0 = min, 1 = hour, 2 = day |
| `intervalAnchorTime` | — | `HH:MM` |
| `camWarmupMs` | — | milliseconds |
| `timedCount` | — | 0–8 |
| `capture_timed_nodes` | `timedNodes` | array, max 8 items: `{ "day": 0–7, "time": "HH:MM:SS" }` (`7` = daily) |

---

## Upload

| Field | Alias | Values |
|-------|-------|--------|
| `upload_mode` | `uploadMode` | `instant`/`0`, `scheduled`/`schedule`/`1` |
| `upload_retry` | `retryCount` | ≥ 0 |
| `upload_timed_count` | — | 0–10 |
| `upload_timed_nodes` | `uploadTimedNodes` | array, max 10 items: `{ "day": 0–7, "time": "HH:MM:SS" }` |

---

## Trigger / PIR

| Field | Values |
|-------|--------|
| `trigger_mode` | 0 = disabled, 1 = alarm input, 2 = PIR |
| `sens` | 0–255 |
| `blind` | 0–15 |
| `pulse` | 0–3 |
| `window` | 0–3 |

---

## Webhook

| Field | Alias | Values |
|-------|-------|--------|
| `webhook_url` | `url` | string, max 255 chars |
| `webhook_header` | `header` | string, max 255 chars |

---

## Push Mode

| Field | Values |
|-------|--------|
| `push_mode` | 0 = MQTT, 1 = Webhook |

---

## Device

| Field | Alias | Values |
|-------|-------|--------|
| `device_name` | `name` | string, max 31 chars |
| `country_code` | `countryCode` | `AU-2020`, `AU-2024`, `AU-revmf`, `US`, `EU`, `GB`, `IN`, `KR`, `NZ`, `SG` (`AU` → `AU-2020`) |

---

## Wi-Fi

| Field | Alias | Values |
|-------|-------|--------|
| `wifi_ssid` | `ssid` | string, max 31 chars |
| `wifi_password` | — | string, max 63 chars |

---

## NTP

| Field | Alias | Values |
|-------|-------|--------|
| `ntp_sync` | `ntpSync` | 0, 1 |
