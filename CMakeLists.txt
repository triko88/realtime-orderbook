cmake_minimum_required(VERSION 3.13)
project(crypto-orderbook)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIRS})

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto nlohmann_json::nlohmann_json)
