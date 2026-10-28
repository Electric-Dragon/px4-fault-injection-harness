//
// Created by Atharva Wasekar on 28.10.26.
//

#include "harness/vehicle/connection_url.hpp"
#include <array>
#include <algorithm>
#include <charconv>

namespace {
    harness::Result<unsigned> parse_number(std::string_view port_str) {

        unsigned value = 0;
        const char* end = port_str.data() + port_str.size();
        auto [ptr, ec] = std::from_chars(port_str.data(), end, value);

        if (ec != std::errc() || ptr != end) {
            return harness::Result<unsigned>::err("Invalid number for port/ baud rate: '" + std::string(port_str) + "'");
        }

        return harness::Result<unsigned>::ok(value);
    }
}

harness::Result<std::string> harness::validate_connection_url(std::string_view url) {

    constexpr std::array protocols = {"udpin://", "udpout://", "tcpin://", "tcpout://", "serial://"};

    if (url.starts_with("udp://")) {
        return Result<std::string>::err("'udp://:<port>' is the legacy form; use udpin://:<port> to listen or udpout://<host>:<port> to send. URL: '" + std::string(url) + "'");
    }

    if (!std::ranges::any_of(protocols, [url](std::string_view protocol) {
        return url.starts_with(protocol);
    })) {
        return Result<std::string>::err("Incorrect protocol name in '" + std::string(url) + "'. Only udpin, udpout, tcpin, tcpout and serial are supported.");
    }

    auto index = url.rfind(':');
    if (index == std::string_view::npos) {
        return Result<std::string>::err("Malformed protocol: '" + std::string(url) + "'");
    }

    auto port_str = url.substr(index+1);
    if (port_str.empty()) {
        return Result<std::string>::err("Missing port/ baud rate: '" + std::string(url) + "'");
    }

    auto parsed_port = parse_number(port_str);

    if (!parsed_port) {
        return Result<std::string>::err(parsed_port.error());
    }

    if (url.starts_with("serial://")) {
        if (parsed_port.value() == 0) {
            return Result<std::string>::err("Baud rate must be greater than zero. Provided: '" + std::string(port_str) + "'");
        }
    } else {
        if (parsed_port.value() == 0 || parsed_port.value() > 65535) {
            return Result<std::string>::err("TCP/UDP ports must be from 1-65535. Provided: '" + std::string(port_str) + "'");
        }
    }

    return Result<std::string>::ok(std::string(url));
}
