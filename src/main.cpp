#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char** argv) {
    CLI::App app{"PX4 fault-injection test harness"};

    std::string specs_dir;
    std::string out_dir = "reports/";
    std::string url = "udpin://:14556";
    std::string filter;

    auto* run_cmd = app.add_subcommand("run", "Run fault-injection test specs");
    run_cmd->add_option("--url", url, "MAVLink connection URL");
    run_cmd->add_option("--specs", specs_dir, "Directory containing test specs")->required();
    run_cmd->add_option("--out", out_dir, "Output directory for reports");
    run_cmd->add_option("--filter", filter, "Glob filter for spec IDs");

    auto* validate_cmd = app.add_subcommand("validate", "Schema-check specs without SITL");
    validate_cmd->add_option("--specs", specs_dir, "Directory containing test specs")->required();

    auto* list_cmd = app.add_subcommand("list", "List available test specs");
    list_cmd->add_option("--specs", specs_dir, "Directory containing test specs")->required();

    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);

    if (run_cmd->parsed()) {
        spdlog::info("run: specs={} url={} out={}", specs_dir, url, out_dir);
    } else if (validate_cmd->parsed()) {
        spdlog::info("validate: specs={}", specs_dir);
    } else if (list_cmd->parsed()) {
        spdlog::info("list: specs={}", specs_dir);
    }

    return 0;
}
