# SITL Docker Environment — Known Constraints

`./docker/run-sitl.sh [px4-tag-or-sha] [sih-target]` — defaults `v1.16.1`, `sihsim_quadx`.
Use `sihsim_airplane` for fixed-wing. Harness side: `--url udpin://:14556`.

- **Simulator is SIH** (PX4's built-in physics): no Gazebo, no Java, headless by nature.
  Verify target names on the pinned ref with `make px4_sitl list_config_targets`.
- **Pin, never float.** Only release tags or full commit SHAs are accepted. The `jmavsim`
  target existed on `main` in 2026-07 and was gone by 2026-10 (`ninja: unknown target 'jmavsim'`).
- **Pin lineage is v1.16.x** so 1.16-era project param sets apply without renames.
- **v1.17.0 uORB codegen** breaks under Python 3.10.
- **Shallow clones need the tag.** Without it PX4 reports `v0.0.0`. The script clones the tag
  itself; a SHA pin has no tag, so record the SHA.
- **Apple Silicon:** amd64 emulation means slow build (~40 min), runtime is fine (lockstep).
  `--rm` means every run rebuilds.
- **Container-to-host MAVLink** needs explicit target IP: harness host LAN IPv4, NOT
  `host.docker.internal` (can resolve IPv6-only). The script adds a MAVLink instance aimed at
  `<host-ip>:14556` once PX4 is up. Bidirectional traffic requires publishing the
  harness-side port: `-p 14556:14556/udp`.
- **PX4 auto-disarms** if armed idle. Arm+takeoff must be one sequence.
- **Param sets are loaded by the harness over MAVLink** (spec §4.5), not baked into the
  container, so the same mechanism works for SITL, HITL and bench.
- **Docker cannot pass USB serial into a container on macOS**, so classic HITL never runs here.
