//
// Created by Atharva Wasekar on 28.10.26.
//

#pragma once
#include "harness/core/result.hpp"
#include <string>
#include <string_view>

namespace harness {

    Result<std::string> validate_connection_url(std::string_view url);

} // namespace harness
