#include <spdlog/spdlog.h>

#include "parquet_analyze_server/parquet_analyze_service_impl.hpp"


#include <absl/flags/flag.h>
#include <absl/flags/parse.h>



ABSL_FLAG(int, port, 42420, "The port the server is supposed to listen on");
ABSL_FLAG(std::string, data_dir, "data", "The directory containing the parquet files");

int main(int argc, char** argv) {
    using namespace parquet_analyze_server;

    spdlog::set_level(spdlog::level::level_enum::info);
    absl::ParseCommandLine(argc, argv);

    auto status = RunServer(absl::GetFlag(FLAGS_port), absl::GetFlag(FLAGS_data_dir));

    if (!status.ok()) {
        spdlog::error(status.message());
        return 1;
    }

    return 0;
}
