
cmake_minimum_required(VERSION 3.20)

project(ThermalEngine LANGUAGES CXX)

# --------------------------------------------------
# General settings
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# Native calculation kernel
# --------------------------------------------------

add_library(thermal_core STATIC
    src/ThermalSystem.cpp
    src/SimulationRunner.cpp
)

target_include_directories(thermal_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# --------------------------------------------------
# Native simulation application
# --------------------------------------------------

add_executable(thermal_app
    src/main.cpp
)

target_link_libraries(thermal_app PRIVATE
    thermal_core
)

# --------------------------------------------------
# .NET hosting dependencies
# --------------------------------------------------

set(DOTNET_HOST_DIR "" CACHE PATH
    "Directory containing .NET hosting headers and libraries"
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

# --------------------------------------------------
# Managed object wrapper
# --------------------------------------------------

add_library(managed_controller STATIC
    src/ManagedController.cpp
)

target_include_directories(managed_controller PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# --------------------------------------------------
# Native .NET host
# --------------------------------------------------

add_executable(native_host
    hosting/NativeHost.cpp
)

target_include_directories(native_host PRIVATE
    "${DOTNET_HOST_INCLUDE_DIR}"
)

target_link_libraries(native_host PRIVATE
    managed_controller
    "${NETHOST_LIBRARY}"
    "${CMAKE_DL_LIBS}"
)

# --------------------------------------------------
# Compiler warnings
# --------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    foreach(target IN ITEMS
        thermal_core
        thermal_app
        managed_controller
        native_host
    )
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
    endforeach()
endif()
