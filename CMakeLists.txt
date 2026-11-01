
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
# Native .NET hosting library
# --------------------------------------------------

add_library(dotnet_host STATIC
    src/DotNetHost.cpp
    src/ManagedController.cpp
)

target_include_directories(dotnet_host
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${DOTNET_HOST_INCLUDE_DIR}"
)

target_link_libraries(dotnet_host PUBLIC
    "${NETHOST_LIBRARY}"
    "${CMAKE_DL_LIBS}"
)

# --------------------------------------------------
# Native simulation application
# --------------------------------------------------

add_executable(thermal_app
    src/main.cpp
)

target_link_libraries(thermal_app PRIVATE
    thermal_core
    dotnet_host
)



# --------------------------------------------------
# Native AOT interoperability library
# --------------------------------------------------

add_library(native_aot_host STATIC
    src/NativeAotHost.cpp
    src/ManagedController.cpp
)

target_include_directories(native_aot_host PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Required for dlopen() and dlsym()
target_link_libraries(native_aot_host PUBLIC
    "${CMAKE_DL_LIBS}"
)

# --------------------------------------------------
# Native AOT test executable
# --------------------------------------------------

add_executable(native_aot_test
    examples/native_aot/main.cpp
)

target_link_libraries(native_aot_test PRIVATE
    native_aot_host
)

# --------------------------------------------------
# Compiler warnings
# --------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(native_aot_host PRIVATE
        -Wall -Wextra -Wpedantic
    )

    target_compile_options(native_aot_test PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    foreach(target IN ITEMS
        thermal_core
        dotnet_host
        thermal_app
    )
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
    endforeach()
endif()
