cmake_minimum_required(VERSION 3.20)
project(thermal-engine LANGUAGES CXX)

add_library(thermal_core SHARED 
    src/ThermalSystem.cpp
    src/SimulationRunner.cpp
)

target_compile_features(thermal_core PUBLIC cxx_std_20)

set_target_properties(thermal_core PROPERTIES 
    CXX_EXTENSIONS OFF
)

target_include_directories(thermal_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(DOTNET_HOST_DIR "" CACHE PATH
    "Directory containing .NET native hosting files"
)

find_path(DOTNET_HOST_INCLUDE_DIR
    NAMES nethost.h
    HINTS "${DOTNET_HOST_DIR}"
    REQUIRED
)

find_library(NETHOST_LIBRARY
    NAMES nethost
    HINTS "${DOTNET_HOST_DIR}"
    REQUIRED
)

add_executable(native_host
    hosting/NativeHost.cpp
)

target_compile_features(native_host PRIVATE cxx_std_20)

target_include_directories(native_host PRIVATE
    "${DOTNET_HOST_INCLUDE_DIR}"
)

target_link_libraries(native_host PRIVATE
    "${NETHOST_LIBRARY}"
    "${CMAKE_DL_LIBS}"
)

add_executable(thermal_app
    src/main.cpp
)

target_link_libraries(thermal_app PRIVATE thermal_core)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(thermal_core PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()