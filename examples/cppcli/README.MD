# C++/CLI Reference Implementation

This directory demonstrates how a native C++ application can access the C# `ThermalController` through C++/CLI.

## Purpose

This is a reference implementation for comparing the original Windows-specific interoperability approach with the Linux-compatible `hostfxr` + Reverse P/Invoke implementation.

## Requirements

- Windows
- Microsoft Visual C++ with C++/CLI support (`/clr`)
- A compatible .NET runtime
- A reference to `ThermalController.dll`

## How it works

The implementation uses `msclr::gcroot` to retain a managed C# object inside a native C++ wrapper.

A native C++ caller can invoke `calculatePower()` without directly interacting with managed types.

## Important

This example is not part of the Linux CMake build and has not been compiled or tested on Windows. It is provided as an illustrative reference.

The working cross-platform implementation is located in the main `src/` and `include/` directories.