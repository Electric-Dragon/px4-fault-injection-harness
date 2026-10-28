//
// Created by Atharva Wasekar on 28.10.26.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "harness/vehicle/connection_url.hpp"

TEST_CASE("Test legacy UDP connection URL is rejected", "[connection_url]") {
    auto r = harness::validate_connection_url("udp://:14540");
    REQUIRE_FALSE(r);
    REQUIRE(r.error().find("udpin") != std::string::npos);
}

TEST_CASE("unknown schemes are rejected", "[connection_url]") {

    auto url = GENERATE(
        "",
        "foo://bar",
        "udpinx://"
    );
    CAPTURE(url);

    auto r = harness::validate_connection_url(url);
    REQUIRE_FALSE(r);

}

TEST_CASE("malformed connection ports are rejected", "[connection_url]") {

    auto url = GENERATE(
        "udpin://",
        "udpin://:",
        "udpin://:abc",
        "udpin://:14a",
        "udpin://:0",
        "udpin://:65536",
        "serial:///dev/cu.usbmodem1"
    );
    CAPTURE(url);

    auto r = harness::validate_connection_url(url);
    REQUIRE_FALSE(r);
}

TEST_CASE("valid schemes are accepted unchanged", "[connection_url]") {
    auto url = GENERATE(
        "udpin://:14540",
        "udpout://192.168.1.5:14550",
        "tcpin://:5760",
        "tcpout://127.0.0.1:5760",
        "serial:///dev/cu.usbmodem1:57600"
    );
    CAPTURE(url);

    auto r = harness::validate_connection_url(url);
    REQUIRE(r);
    REQUIRE(r.value() == url);
}

