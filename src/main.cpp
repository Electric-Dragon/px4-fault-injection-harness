#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>
#include "harness/vehicle/connection_url.hpp"
#include <mavsdk/mavsdk.h>

#include "spdlog/fmt/bundled/ostream.h"

int main(int argc, char** argv) {
    CLI::App app{"PX4 fault-injection test harness"};

    std::string specs_dir;
    std::string out_dir = "reports/";
    std::string url = "udpin://0.0.0.0:14556";
    std::string filter;
    double timeout = 10.0;

    auto* run_cmd = app.add_subcommand("run", "Run fault-injection test specs");
    run_cmd->add_option("--url", url, "MAVLink connection URL");
    run_cmd->add_option("--specs", specs_dir, "Directory containing test specs"); //->required();
    run_cmd->add_option("--out", out_dir, "Output directory for reports");
    run_cmd->add_option("--filter", filter, "Glob filter for spec IDs");
    run_cmd->add_option("--timeout", timeout, "Timeout to find autopilot in seconds")->check(CLI::PositiveNumber);

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
        mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(url);
        if (connection_result != mavsdk::ConnectionResult::Success) {
            spdlog::error("Connection failed for {}: {}", url,fmt::streamed(connection_result));
            return 1;
        }

        auto first_autopilot = mavsdk.first_autopilot(timeout);
        if (!first_autopilot) {
            spdlog::error("No autopilot found on '{}' in {}s",url, timeout );
            return 1;
        }

        const auto& system = first_autopilot.value();

        spdlog::info("System ID: {}", system->get_system_id());


    } else if (validate_cmd->parsed()) {
        spdlog::info("validate: specs={}", specs_dir);
    } else if (list_cmd->parsed()) {
        spdlog::info("list: specs={}", specs_dir);
    }

    return 0;
}
