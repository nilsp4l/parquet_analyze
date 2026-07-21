#include "parquet_analyze_cli/parquet_analyze_flight_client.hpp"


namespace parquet_analyze_cli {
    ParquetAnalyzeFlightClient::ParquetAnalyzeFlightClient(const std::string &host, int port) {
        if (auto status = initInternalClient(host, port); !status.ok()) {
            throw std::runtime_error(status.message());
        }
    }

    arrow::Status ParquetAnalyzeFlightClient::initInternalClient(const std::string &host, const int port) {
        arrow::flight::Location location;
        ARROW_ASSIGN_OR_RAISE(location,
                              arrow::flight::Location::ForGrpcTcp(host, port));

        ARROW_ASSIGN_OR_RAISE(internal_client_, arrow::flight::FlightClient::Connect(location));

        return arrow::Status::OK();
    }


    arrow::Result<std::unique_ptr<arrow::flight::FlightInfo> > ParquetAnalyzeFlightClient::GetFlightInfo(
        const arrow::flight::FlightDescriptor &descriptor) const {
        return internal_client_->GetFlightInfo(descriptor);
    }

    arrow::Result<std::unique_ptr<arrow::flight::FlightStreamReader> > ParquetAnalyzeFlightClient::DoGet(
        const arrow::flight::Ticket &ticket) const {
        return internal_client_->DoGet(ticket);
    }
}
