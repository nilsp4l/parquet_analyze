#ifndef PARQUET_ANALYZE_CLI_METADATA_SERVICE_HPP
#define PARQUET_ANALYZE_CLI_METADATA_SERVICE_HPP


#include "parquet_analyze_cli/parquet_analyze_flight_client.hpp"

#define PRETTY_PRINT_OPTIONS_PARAMS 0, 2

class MetaDataService {
public:

    MetaDataService(const std::string &host, int port);

    std::string summary(const std::string &dataset_name, const std::string &column_name);

    std::string info(const std::string &dataset_name);

private:

    arrow::Status summary_internal(const std::string &dataset_name, const std::string &column_name, std::string &summary);

    arrow::Status info_internal(const std::string &dataset_name, std::string &info);

    std::unique_ptr<parquet_analyze_cli::ParquetAnalyzeFlightClient> internal_client_;
};

#endif //PARQUET_ANALYZE_CLI_METADATA_SERVICE_HPP