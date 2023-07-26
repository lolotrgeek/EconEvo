cmake_minimum_required(VERSION 3.0.0)
project(EconEvo VERSION 0.1.0 LANGUAGES C CXX)

INCLUDE_DIRECTORIES(C:/vcpkg/installed/x64-windows/include)
LINK_DIRECTORIES(C:/vcpkg/installed/x64-windows/lib)

add_executable(EconEvo main.cpp)

find_package(cppzmq CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
TARGET_LINK_LIBRARIES(EconEvo 
    cppzmq cppzmq-static nlohmann_json::nlohmann_json
)