# Test Spec Format (v0)

```yaml
id: GPS_LOSS_MISSION_01
description: Full GPS loss in mission triggers position failsafe within bound
precondition: { mode: AUTO.MISSION, min_alt_m: 20, settle_s: 5 }
fault: { injector: sitl, unit: gps, instance: 0, failure: "off", at_s: 5 }
expect:
  - { event: failsafe_flag, flag: global_position_invalid, within: { param: COM_POS_FS_DELAY, plus_s: 1.0 } }
  - { event: mode_change, to_any: [AUTO.RTL, DESCEND, AUTO.LAND], within: { fixed_s: 10 } }
  - { prohibited: disarmed, window_s: 20 }
teardown: { restore: all, land: true, disarm: true }
```

See spec.md S5 for the full specification.
