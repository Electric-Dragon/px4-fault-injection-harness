#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/catch_tostring.hpp>
#include "harness/vehicle/vehicle_class.hpp"

using harness::VehicleClass;
using mavsdk::Vehicle;

// Catch prints values via operator<< when one is declared. MAVSDK 3.17.4 declares one for
// Vehicle but doesn't export it (link error), so route both enums through our to_string.
template <>
struct Catch::StringMaker<Vehicle> {
    static std::string convert(Vehicle v) { return std::string(harness::to_string(v)); }
};
template <>
struct Catch::StringMaker<VehicleClass> {
    static std::string convert(VehicleClass c) { return std::string(harness::to_string(c)); }
};

TEST_CASE("supported vehicle types map to their class", "[vehicle_class]") {

    auto [vehicle, expected] = GENERATE(table<Vehicle, VehicleClass>({
        {Vehicle::FixedWing, VehicleClass::FixedWing},
        {Vehicle::Quadrotor, VehicleClass::Multicopter},
        {Vehicle::Hexarotor, VehicleClass::Multicopter},
        {Vehicle::Octorotor, VehicleClass::Multicopter},
        {Vehicle::Tricopter, VehicleClass::Multicopter},
        {Vehicle::Dodecarotor, VehicleClass::Multicopter},
        {Vehicle::Decarotor, VehicleClass::Multicopter},
        {Vehicle::GenericMultirotor, VehicleClass::Multicopter},
    }));
    CAPTURE(vehicle);

    auto r = harness::vehicle_class_from(vehicle);
    REQUIRE(r);
    CHECK(r.value() == expected);
}

TEST_CASE("unsupported vehicle types are rejected with their name", "[vehicle_class]") {

    auto vehicle = GENERATE(
        Vehicle::Unknown,
        Vehicle::Generic,
        Vehicle::Helicopter,
        Vehicle::Coaxial,
        Vehicle::GroundRover,
        Vehicle::Submarine,
        Vehicle::VtolTiltrotor,
        Vehicle::VtolTailsitterQuadrotor,
        Vehicle::VtolFixedrotor
    );
    CAPTURE(vehicle);

    auto r = harness::vehicle_class_from(vehicle);
    REQUIRE_FALSE(r);
    CHECK(r.error().find(harness::to_string(vehicle)) != std::string::npos);
}

TEST_CASE("to_string names every vehicle class", "[vehicle_class]") {
    CHECK(harness::to_string(VehicleClass::FixedWing) == "FixedWing");
    CHECK(harness::to_string(VehicleClass::Multicopter) == "Multicopter");
}
