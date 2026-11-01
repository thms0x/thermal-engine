
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

# Shared by the native, hostfxr and Native AOT apps
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

# Directory containing nethost.h and libnethost
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
# hostfxr interoperability library
# --------------------------------------------------

# Initializes CoreCLR and provides managed entry points
add_library(dotnet_host STATIC
    examples/hostfxr/DotNetHost.cpp
    examples/hostfxr/ManagedController.cpp
)

target_include_directories(dotnet_host
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/examples/hostfxr"
    PRIVATE "${DOTNET_HOST_INCLUDE_DIR}"
)

target_link_libraries(dotnet_host PUBLIC
    "${NETHOST_LIBRARY}"
    "${CMAKE_DL_LIBS}"
)

# --------------------------------------------------
# hostfxr simulation application
# --------------------------------------------------

add_executable(thermal_hostfxr_app
    examples/hostfxr/main.cpp
)

target_link_libraries(thermal_hostfxr_app PRIVATE
    thermal_core
    dotnet_host
)

# --------------------------------------------------
# Native AOT interoperability library
# --------------------------------------------------

# Loads Native AOT exports directly using dlopen/dlsym
add_library(native_aot_host STATIC
    examples/native_aot/NativeAotHost.cpp
    examples/native_aot/ManagedController.cpp
)

target_include_directories(native_aot_host PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/native_aot"
)

target_link_libraries(native_aot_host PUBLIC
    "${CMAKE_DL_LIBS}"
)

# --------------------------------------------------
# Native AOT simulation application
# --------------------------------------------------

add_executable(thermal_aot_app
    examples/native_aot/main.cpp
)

target_link_libraries(thermal_aot_app PRIVATE
    thermal_core
    native_aot_host
)

# --------------------------------------------------
# Compiler warnings
# --------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    foreach(target IN ITEMS
        thermal_core
        thermal_app
        dotnet_host
        thermal_hostfxr_app
        native_aot_host
        thermal_aot_app
    )
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
    endforeach()
endif()

# --------------------------------------------------
# C++/CLI reference
# --------------------------------------------------

# Windows-only illustrative implementation.
# Intentionally excluded from the Linux build.
#
# See examples/cppcli/README.md
