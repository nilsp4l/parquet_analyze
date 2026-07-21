#ifndef PARQUET_ANALYZE_CLI_DATADIGGER_CLIENT_HPP
#define PARQUET_ANALYZE_CLI_DATADIGGER_CLIENT_HPP

#include "arrow/flight/client.h"

namespace parquet_analyze_cli {
    class ParquetAnalyzeFlightClient {
    public:

        ParquetAnalyzeFlightClient(const std::string& host, int port);

        // only expose the implemented endpoints
        arrow::Result<std::unique_ptr<arrow::flight::FlightInfo> > GetFlightInfo(
            const arrow::flight::FlightDescriptor &descriptor) const;

        arrow::Result<std::unique_ptr<arrow::flight::FlightStreamReader>> DoGet(const arrow::flight::Ticket& ticket) const;

    private:

        arrow::Status initInternalClient(const std::string& host, int port);

        std::unique_ptr<arrow::flight::FlightClient> internal_client_;
    };
}

#endif //PARQUET_ANALYZE_CLI_DATADIGGER_CLIENT_HPP
