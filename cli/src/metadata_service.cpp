#include "parquet_analyze_cli/metadata_service.hpp"

#include <arrow/pretty_print.h>

#include "query.pb.h"

MetaDataService::MetaDataService(const std::string &host, int port) : internal_client_(
    std::make_unique<parquet_analyze_cli::ParquetAnalyzeFlightClient>(host, port)) {
}

std::string MetaDataService::summary(const std::string &dataset_name, const std::string &column_name) {
    std::string summary{};
    if (auto status = summary_internal(dataset_name, column_name, summary); !status.ok()) {
        throw std::runtime_error(status.message());
    }
    return summary;
}

std::string MetaDataService::info(const std::string &dataset_name) {
    std::string info{};
    if (auto status = info_internal(dataset_name, info); !status.ok()) {
        throw std::runtime_error(status.message());
    }
    return info;
}

arrow::Status MetaDataService::summary_internal(const std::string &dataset_name, const std::string &column_name,
    std::string &summary) {

    std::unique_ptr<arrow::flight::FlightStreamReader> reader;

    Query query;
    query.set_dataset_name(dataset_name);
    query.set_column_name((column_name));
    ARROW_ASSIGN_OR_RAISE(reader, internal_client_->DoGet(arrow::flight::Ticket{query.SerializeAsString()}));

    std::shared_ptr<arrow::Table> table;

    ARROW_ASSIGN_OR_RAISE(table, reader->ToTable());
    arrow::PrettyPrintOptions print_options(PRETTY_PRINT_OPTIONS_PARAMS);
    std::stringstream summary_stream;
    ARROW_RETURN_NOT_OK(arrow::PrettyPrint(*table, print_options, &summary_stream));
    summary = summary_stream.str();

    return arrow::Status::OK();
}


// from https://arrow.apache.org/cookbook/cpp/flight.html
arrow::Status MetaDataService::info_internal(const std::string &dataset_name, std::string &info) {
    info = "";
    std::unique_ptr<arrow::flight::FlightInfo> result;
    ARROW_ASSIGN_OR_RAISE(result, internal_client_->GetFlightInfo(arrow::flight::FlightDescriptor::Path({dataset_name})));
    info += result->descriptor().ToString() + "\n";
    info += std::string{"=== Schema ==="} + "\n";
    std::shared_ptr<arrow::Schema> info_schema;
    arrow::ipc::DictionaryMemo dictionary_memo;
    ARROW_ASSIGN_OR_RAISE(info_schema, result->GetSchema(&dictionary_memo));
    info += info_schema->ToString() + "\n";
    info += std::string{"=============="} + "\n";

    return arrow::Status::OK();
}
