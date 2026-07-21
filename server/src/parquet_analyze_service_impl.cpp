#include <memory>

#include "parquet_analyze_server/parquet_analyze_service_impl.hpp"
#include <arrow/compute/api.h>

#include <csignal>
#include <arrow/flight/server.h>
#include <arrow/filesystem/api.h>
#include <parquet/arrow/reader.h>
#include <arrow/dataset/dataset.h>
#include "arrow/table.h"

#include "query.pb.h"
#include <arrow/dataset/api.h>
#include <spdlog/spdlog.h>
#include "grpcpp/server_builder.h"


namespace parquet_analyze_server {
    class ParquetAnalyzeServiceImpl : public arrow::flight::FlightServerBase {
    public:
        explicit ParquetAnalyzeServiceImpl(std::shared_ptr<arrow::fs::FileSystem> root)
            : root_(std::move(root)) {
        }

        arrow::Status GetFlightInfo(const arrow::flight::ServerCallContext &,
                                    const arrow::flight::FlightDescriptor &descriptor,
                                    std::unique_ptr<arrow::flight::FlightInfo> *info) override;

        arrow::Status DoGet(const arrow::flight::ServerCallContext &, const arrow::flight::Ticket &request,
                            std::unique_ptr<arrow::flight::FlightDataStream> *stream) override;

    private:
        arrow::Result<arrow::flight::FlightInfo> MakeFlightInfo(const arrow::fs::FileInfo &file_info);


        arrow::Result<arrow::fs::FileInfo> FileInfoFromDescriptor(
            const arrow::flight::FlightDescriptor &descriptor);

        std::shared_ptr<arrow::fs::FileSystem> root_;
    };

    arrow::Status ParquetAnalyzeServiceImpl::DoGet(const arrow::flight::ServerCallContext &,
                                                   const arrow::flight::Ticket &request,
                                                   std::unique_ptr<arrow::flight::FlightDataStream> *stream) {
        Query query;
        if (!query.ParseFromString(request.ticket)) {
            return arrow::Status::SerializationError(std::format("Could not deserialize query {}", request.ticket));
        }

        spdlog::info(
            std::format("Received summary query on column {} in {}", query.column_name(), query.dataset_name()));

        arrow::fs::FileSelector selector;

        auto format = std::make_shared<arrow::dataset::ParquetFileFormat>();

        // we do all this scanner stuff for the sole purpose of not wasting resources by loading the table
        // and filter afterward. Filtering before loading seems far more reasonable.
        ARROW_ASSIGN_OR_RAISE(auto factory, arrow::dataset::FileSystemDatasetFactory::Make(root_, selector, format,
                                  arrow::dataset::FileSystemFactoryOptions()));

        ARROW_ASSIGN_OR_RAISE(auto dataset, factory->Finish());


        ARROW_ASSIGN_OR_RAISE(auto scan_builder, dataset->NewScan());

        ARROW_RETURN_NOT_OK(scan_builder->Project({query.column_name()}));

        ARROW_ASSIGN_OR_RAISE(auto scanner, scan_builder->Finish());


        std::shared_ptr<arrow::Table> table;
        ARROW_ASSIGN_OR_RAISE(table, scanner->ToTable());

        ARROW_ASSIGN_OR_RAISE(auto reader, scanner->ToRecordBatchReader());


        *stream = std::make_unique<arrow::flight::RecordBatchStream>(reader);
        return arrow::Status::OK();
    }


    // from https://arrow.apache.org/cookbook/cpp/flight.html {
    arrow::Status ParquetAnalyzeServiceImpl::GetFlightInfo(const arrow::flight::ServerCallContext &,
                                                           const arrow::flight::FlightDescriptor &descriptor,
                                                           std::unique_ptr<arrow::flight::FlightInfo> *info) {
        spdlog::info(std::format("Creating info for descriptor: {}", descriptor.ToString()));
        ARROW_ASSIGN_OR_RAISE(auto file_info, FileInfoFromDescriptor(descriptor));

        ARROW_ASSIGN_OR_RAISE(auto flight_info, MakeFlightInfo(file_info));

        *info = std::make_unique<arrow::flight::FlightInfo>(std::move(flight_info));

        return arrow::Status::OK();
    }


    arrow::Result<arrow::flight::FlightInfo>
    ParquetAnalyzeServiceImpl::MakeFlightInfo(const arrow::fs::FileInfo &file_info) {
        ARROW_ASSIGN_OR_RAISE(auto input, root_->OpenInputFile(file_info));

        std::unique_ptr<parquet::arrow::FileReader> reader;
        ARROW_ASSIGN_OR_RAISE(reader, parquet::arrow::OpenFile(std::move(input), arrow::default_memory_pool()));

        std::shared_ptr<arrow::Schema> schema;
        ARROW_RETURN_NOT_OK(reader->GetSchema(&schema));

        auto descriptor = arrow::flight::FlightDescriptor::Path({file_info.base_name()});

        arrow::flight::FlightEndpoint endpoint;

        endpoint.ticket.ticket = file_info.base_name();


        arrow::flight::Location location;

        // keep this at localhost for now for the sake of simplicity assuming we are on the same machine
        ARROW_ASSIGN_OR_RAISE(location, arrow::flight::Location::ForGrpcTcp("localhost", port()));

        endpoint.locations.push_back(location);

        int64_t total_records = reader->parquet_reader()->metadata()->num_rows();
        int64_t total_bytes = file_info.size();

        return arrow::flight::FlightInfo::Make(*schema, descriptor, {endpoint}, total_records,
                                               total_bytes);
    }

    arrow::Result<arrow::fs::FileInfo> ParquetAnalyzeServiceImpl::FileInfoFromDescriptor(
        const arrow::flight::FlightDescriptor &descriptor) {
        if (descriptor.type != arrow::flight::FlightDescriptor::PATH) {
            return arrow::Status::Invalid("Must provide PATH-type FlightDescriptor");
        }

        if (descriptor.path.size() != 1) {
            return arrow::Status::Invalid(
                "Must provide PATH-type FlightDescriptor with one path component");
        }

        return root_->GetFileInfo(descriptor.path[0]);
    }

    // }

    arrow::Status RunServer(int port, const std::string &data_dir) {
        spdlog::info("Starting server");
        auto fs = std::make_shared<arrow::fs::LocalFileSystem>();
        ARROW_RETURN_NOT_OK(fs->CreateDir(data_dir));

        auto root = std::make_shared<arrow::fs::SubTreeFileSystem>(data_dir, fs);

        arrow::flight::Location server_location;
        ARROW_ASSIGN_OR_RAISE(server_location,
                              arrow::flight::Location::ForGrpcTcp("0.0.0.0", port));

        arrow::flight::FlightServerOptions options(server_location);
        auto server = std::unique_ptr<arrow::flight::FlightServerBase>(
            std::make_unique<ParquetAnalyzeServiceImpl>(std::move(root)));
        ARROW_RETURN_NOT_OK(server->Init(options));
        ARROW_RETURN_NOT_OK(server->SetShutdownOnSignals({SIGTERM}));

        ARROW_RETURN_NOT_OK(arrow::compute::Initialize());

        spdlog::info(std::format("Listening on port {}", port));

        return server->Serve();
    }
}
