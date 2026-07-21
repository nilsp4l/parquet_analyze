#ifndef PARQUET_ANALYZE_SERVER_PARQUET_ANALYZE_SERVICE_IMPL_HPP
#define PARQUET_ANALYZE_SERVER_PARQUET_ANALYZE_SERVICE_IMPL_HPP

#include <arrow/status.h>

namespace parquet_analyze_server {

    arrow::Status RunServer(int port, const std::string &data_dir);
}


#endif //PARQUET_ANALYZE_SERVER_PARQUET_ANALYZE_SERVICE_IMPL_HPP
