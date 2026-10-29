#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "harness/vehicle/connection_url.hpp"
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/info/info.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include "harness/vehicle/vehicle_class.hpp"

namespace {
    std::atomic<bool> killed{false};
    void signal_handler(int) {
        killed = true;
    }
}

using namespace std::chrono;
int main(int argc, char** argv) {
    CLI::App app{"PX4 fault-injection test harness"};

    std::string specs_dir;
    std::string out_dir = "reports/";
    std::string url = "udpin://0.0.0.0:14556";
    std::string filter;
    double timeout = 30.0;

    auto* run_cmd = app.add_subcommand("run", "Run fault-injection test specs");
    run_cmd->add_option("--url", url, "MAVLink connection URL");
    run_cmd->add_option("--specs", specs_dir, "Directory containing test specs"); //->required();
    run_cmd->add_option("--out", out_dir, "Output directory for reports");
    run_cmd->add_option("--filter", filter, "Glob filter for spec IDs");
    run_cmd->add_option("--timeout", timeout, "Timeout to find autopilot and get vehicle info for initialization in seconds")->check(CLI::PositiveNumber);

    auto* validate_cmd = app.add_subcommand("validate", "Schema-check specs without SITL");
    validate_cmd->add_option("--specs", specs_dir, "Directory containing test specs")->required();

    auto* list_cmd = app.add_subcommand("list", "List available test specs");
    list_cmd->add_option("--specs", specs_dir, "Directory containing test specs")->required();

    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);

    if (run_cmd->parsed()) {

        spdlog::info("run: specs={} url={} out={}", specs_dir, url, out_dir);

        if (auto url_validation_result = harness::validate_connection_url(url); !url_validation_result) {
            spdlog::error("{}", url_validation_result.error());
            return 1;
        }

        mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};
        const mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(url);
        if (connection_result != mavsdk::ConnectionResult::Success) {
            spdlog::error("Connection failed for {}: {}", url,fmt::streamed(connection_result));
            return 1;
        }

        const auto timeout_start = steady_clock::now();
        const auto timeout_deadline = timeout_start + duration<double>(timeout);

        const auto first_autopilot = mavsdk.first_autopilot(timeout);
        if (!first_autopilot) {
            spdlog::error("No autopilot found on '{}' in {}s",url, timeout );
            return 1;
        }

        const auto& system = first_autopilot.value();

        const auto autopilot_type = system->autopilot_type();

        if (autopilot_type != mavsdk::Autopilot::Px4) {
            spdlog::error("Only PX4 autopilots are supported");
            return 1;
        }

        spdlog::info("System ID: {}", system->get_system_id());

        const auto vehicle_type = system->vehicle_type();

        spdlog::info("Vehicle type: {}",harness::to_string(vehicle_type));

        const auto vehicle_class_result = harness::vehicle_class_from(vehicle_type);

        if (!vehicle_class_result) {
            spdlog::error("{}",vehicle_class_result.error());
            return 1;
        }

        spdlog::info("Vehicle class: {}", harness::to_string(vehicle_class_result.value()));

        auto info = mavsdk::Info{system};

        while (info.get_version().first==mavsdk::Info::Result::InformationNotReceivedYet) {

            if (steady_clock::now() >= timeout_deadline) {
                spdlog::error("Failed to fetch system version in {}s", timeout);
                return 1;
            }

            std::this_thread::sleep_for(milliseconds(100));
        }

        const auto [system_version_result, version] = info.get_version();

        if (system_version_result != mavsdk::Info::Result::Success) {
            spdlog::error("Failed to get system version: {}", fmt::streamed(system_version_result));
            return 1;
        }

        spdlog::info("System version: {}.{}.{}",version.flight_sw_major, version.flight_sw_minor, version.flight_sw_patch);

        auto telemetry = mavsdk::Telemetry{system};
        auto flight_mode = telemetry.flight_mode();

        spdlog::info("Flight mode: {}", fmt::streamed(flight_mode));

        auto telemetry_status_text_listener = telemetry.subscribe_status_text([](const mavsdk::Telemetry::StatusText& status_text) {
            switch (status_text.type) {
                case mavsdk::Telemetry::StatusTextType::Alert:
                case mavsdk::Telemetry::StatusTextType::Emergency:
                case mavsdk::Telemetry::StatusTextType::Critical:
                case mavsdk::Telemetry::StatusTextType::Error:
                    spdlog::error("PX4: {}", status_text.text);
                    break;
                case mavsdk::Telemetry::StatusTextType::Debug:
                    spdlog::debug("PX4: {}", status_text.text);
                    break;
                case mavsdk::Telemetry::StatusTextType::Info:
                case mavsdk::Telemetry::StatusTextType::Notice:
                    spdlog::info("PX4: {}", status_text.text);
                    break;
                case mavsdk::Telemetry::StatusTextType::Warning:
                    spdlog::warn("PX4: {}", status_text.text);
                    break;
            }
        });

        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        while (!killed) {
            std::this_thread::sleep_for(milliseconds(100));
        }

        spdlog::info("Ending harness...");

        telemetry.unsubscribe_status_text(telemetry_status_text_listener);

    } else if (validate_cmd->parsed()) {
        spdlog::info("validate: specs={}", specs_dir);
    } else if (list_cmd->parsed()) {
        spdlog::info("list: specs={}", specs_dir);
    }

    return 0;
}
