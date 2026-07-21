#include <iostream>
#include <string>
#include <spdlog/spdlog.h>

#include "parquet_analyze_cli/parquet_analyze_flight_client.hpp"
#include "parquet_analyze_cli/metadata_service.hpp"
#include <absl/flags/flag.h>
#include <absl/flags/parse.h>

ABSL_FLAG(int, port, 42420, "Port of the server to connect to");
ABSL_FLAG(std::string, host, "localhost", "Host of the server to connect to");
ABSL_FLAG(std::string, data, "data.parquet", "Data to retrieve metadata from");
ABSL_FLAG(bool, info, false, "Exec the info command");
ABSL_FLAG(std::optional<std::string>, summarize, std::nullopt, "Exec the summarize command on the argument's column name");

int main(int argc, char **argv) {
    using namespace parquet_analyze_cli;
    spdlog::set_level(spdlog::level::level_enum::info);
    absl::ParseCommandLine(argc, argv);

    try {
        auto metadata_service{MetaDataService(absl::GetFlag(FLAGS_host), absl::GetFlag(FLAGS_port))};

        if (absl::GetFlag(FLAGS_info)) {
            std::cout << metadata_service.info(absl::GetFlag(FLAGS_data)) << std::endl;
        }
        else if (absl::GetFlag(FLAGS_summarize).has_value()) {
            std::cout << metadata_service.summary(absl::GetFlag(FLAGS_data), absl::GetFlag(FLAGS_summarize).value());
        }

    } catch (std::exception &e) {
        spdlog::error(e.what());
    }
    return 0;
}
