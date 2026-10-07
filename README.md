# px4-fault-harness

Generic C++ fault-injection test harness for PX4 SITL.
Spec-driven (YAML), MAVSDK-based, with a MAVLink MITM proxy for link-fault injection.

## Quickstart

```bash
brew install mavsdk
cmake --preset dev && cmake --build --preset dev
ctest --preset dev
```

## With SITL

```bash
docker/run-sitl.sh
./build/harness run --specs specs/examples --out reports/
```

## License

MIT
