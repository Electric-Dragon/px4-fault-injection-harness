//
// Created by Atharva Wasekar on 29.10.26.
//

#pragma once
#include <mavsdk/vehicle.h>
#include <string_view>
#include <string>

#include "harness/core/result.hpp"

namespace harness {
    enum class VehicleClass {FixedWing, Multicopter};

    std::string_view to_string(VehicleClass);
    std::string_view to_string(mavsdk::Vehicle);
    Result<VehicleClass> vehicle_class_from(mavsdk::Vehicle);
}
