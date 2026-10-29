//
// Created by Atharva Wasekar on 29.10.26.
//

#include <spdlog/fmt/ostr.h>
#include "harness/vehicle/vehicle_class.hpp"


namespace harness {

    std::string_view to_string(harness::VehicleClass vehicle_class) {
        switch (vehicle_class) {
            case harness::VehicleClass::FixedWing:
                return "FixedWing";
            case harness::VehicleClass::Multicopter:
                return "Multicopter";
        }
        return "Unknown";
    }

    std::string_view to_string(mavsdk::Vehicle vehicle) {
        switch (vehicle) {
            case mavsdk::Vehicle::Unknown:
                return "Unknown";
            case mavsdk::Vehicle::Generic:
                return "Generic";
            case mavsdk::Vehicle::FixedWing:
                return "FixedWing";
            case mavsdk::Vehicle::Quadrotor:
                return "Quadrotor";
            case mavsdk::Vehicle::Coaxial:
                return "Coaxial";
            case mavsdk::Vehicle::Helicopter:
                return "Helicopter";
            case mavsdk::Vehicle::Airship:
                return "Airship";
            case mavsdk::Vehicle::FreeBalloon:
                return "FreeBalloon";
            case mavsdk::Vehicle::Rocket:
                return "Rocket";
            case mavsdk::Vehicle::GroundRover:
                return "GroundRover";
            case mavsdk::Vehicle::SurfaceBoat:
                return "SurfaceBoat";
            case mavsdk::Vehicle::Submarine:
                return "Submarine";
            case mavsdk::Vehicle::Hexarotor:
                return "Hexarotor";
            case mavsdk::Vehicle::Octorotor:
                return "Octorotor";
            case mavsdk::Vehicle::Tricopter:
                return "Tricopter";
            case mavsdk::Vehicle::FlappingWing:
                return "FlappingWing";
            case mavsdk::Vehicle::Kite:
                return "Kite";
            case mavsdk::Vehicle::VtolTailsitterDuorotor:
                return "VtolTailsitterDuorotor";
            case mavsdk::Vehicle::VtolTailsitterQuadrotor:
                return "VtolTailsitterQuadrotor";
            case mavsdk::Vehicle::VtolTiltrotor:
                return "VtolTiltrotor";
            case mavsdk::Vehicle::VtolFixedrotor:
                return "VtolFixedrotor";
            case mavsdk::Vehicle::VtolTailsitter:
                return "VtolTailsitter";
            case mavsdk::Vehicle::VtolTiltwing:
                return "VtolTiltwing";
            case mavsdk::Vehicle::Parafoil:
                return "Parafoil";
            case mavsdk::Vehicle::Dodecarotor:
                return "Dodecarotor";
            case mavsdk::Vehicle::Decarotor:
                return "Decarotor";
            case mavsdk::Vehicle::Parachute:
                return "Parachute";
            case mavsdk::Vehicle::GenericMultirotor:
                return "GenericMultirotor";
        }
        return "Unknown";
    }

    Result<VehicleClass> vehicle_class_from(mavsdk::Vehicle vehicle) {
        switch (vehicle) {
            case mavsdk::Vehicle::FixedWing:
                return Result<VehicleClass>::ok(VehicleClass::FixedWing);

            case mavsdk::Vehicle::Quadrotor:
            case mavsdk::Vehicle::Hexarotor:
            case mavsdk::Vehicle::Octorotor:
            case mavsdk::Vehicle::Tricopter:
            case mavsdk::Vehicle::Dodecarotor:
            case mavsdk::Vehicle::Decarotor:
            case mavsdk::Vehicle::GenericMultirotor:
                return Result<VehicleClass>::ok(VehicleClass::Multicopter);

            default:
                return Result<VehicleClass>::err(fmt::format("vehicle type {} is not supported", to_string(vehicle)));
        }
    }

}
