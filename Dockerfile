FROM ubuntu:latest AS builder

RUN apt-get update && apt-get install -y \
    make cmake build-essential git \
    libabsl-dev \
    libarrow-dev libarrow-flight-dev libparquet-dev libarrow-dataset-dev libarrow-acero-dev

WORKDIR /src
COPY . .

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

FROM ubuntu:latest AS base

RUN apt-get update && apt-get install -y \
    libabsl-dev \
    libarrow-dev libarrow-flight-dev libparquet-dev libarrow-dataset-dev libarrow-acero-dev

EXPOSE 42420

FROM base AS client
COPY --from=builder /src/build/cli/parquet_analyze_cli /usr/local/bin
ENTRYPOINT ["usr/local/bin/parquet_analyze_cli"]

FROM base AS server
COPY --from=builder /src/build/server/parquet_analyze_server /usr/local/bin
COPY --from=builder /src/data ./data
ENTRYPOINT ["usr/local/bin/parquet_analyze_server"]
