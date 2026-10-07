# SITL Docker Environment — Known Constraints

- **v1.17.0 uORB codegen** breaks under Python 3.10. Default pin: latest tag that builds. `main` @ 2026-07 known-good with jmavsim.
- **Apple Silicon:** amd64 emulation means slow build (~40 min), runtime is fine (lockstep).
- **Container-to-host MAVLink** needs explicit target IP: harness host LAN IPv4, NOT `host.docker.internal` (can resolve IPv6-only). Bidirectional traffic requires publishing the harness-side port: `-p 14556:14556/udp`.
- **PX4 auto-disarms** if armed idle. Arm+takeoff must be one sequence.
- **Param loading:** support `PX4_PARAM_FILE`-style param loading for per-project param sets.
