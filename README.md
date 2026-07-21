# parquet_analyze 

Retrieve metadata / data from parquet files.

Note that the build and run instructions are given for Debian based systems. For other distros
you need to translate them accordingly.


## Prerequisites
You need to install Apache Arrow globally:
https://arrow.apache.org/install/

Installing Arrow Dataset, Arrow Flight and Parquet should be sufficient.

Furthermore, you will need to install Abseil and Protobuf

```
sudo apt install libabsl-dev
sudo apt install libprotoc-dev
```
Note that each of these packages potentially requires prerequisites on their own.

## Building
You may build everything with the standard CMake build procedure.
```
mkdir build && cd build
cmake ..
make
```
In this step the contents of the data directory are copied to the server subdirectory.
This potentially makes it easier to address them later on. If you don't wish for that to happen
you can turn off the CMake option COPY_DATA.


## Running the Server
The binary for the server lies in the server subdirectory of your build directory.
You can see the possible flags using the flag --helpfull.

### Example Usage:

```
./parquet_analyze_server --data_dir "data" --port 42420
```

If you're seeing 

```
[SOME DATE] [info] Starting server
[SOME DATE] [info] Listening on port 42420
```

you should be good to go.

Note that the data_dir path is relative to your pwd. So adapt accordingly.

## Running the cli
The binary for the server lies in the server subdirectory of your build directory.
You can see the possible flags using the flag --helpfull here as well.

You have two possible commands: 

- info (Get Metadata of the dataset)
- summarize (Get a summary of a specified column on the dataset)

Obviously you need to have a running server on the given host and port for the cli to work.

### Example Usage

#### Info
```
./parquet_analyze_cli --data "data.parquet" --host "localhost" --port 42420 --info
```

#### Summarize

```
./parquet_analyze_cli --data "data.parquet" --host "localhost" --port 42420 --summarize "x_0"
```

