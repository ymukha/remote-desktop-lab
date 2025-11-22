# Remote Desktop Lab

Experimental remote desktop application written in modern C++ as a portfolio project.

The goal of this project is to build a minimal but realistic remote connectivity prototype:
a lightweight alternative to classic RDP-style solutions, inspired by tools like TeamViewer.

## High-level architecture

The project is split into three main components:

- `core/` – shared C++ core:
  - frame capture abstraction (GDI / DXGI on Windows first)
  - compression / delta encoding
  - binary protocol definitions
  - logging and basic utilities

- `server-qt/` – Qt-based desktop server:
  - captures the local screen
  - encodes frames and sends them to the client
  - receives input events (mouse/keyboard) and injects them into the OS

- `client-qt/` – Qt-based desktop client:
  - connects to the server over TCP
  - receives and decodes frames
  - displays the remote desktop
  - sends user input back to the server

## Build

### Install vcpkg

mkdir <some_path>\dev
cd <some_path>\dev
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat

<some_path>\dev\vcpkg will be <vcpkg_path>. There is already vcpkg.exe placed

### Build Qt

Run inside <vcpkg_path> in Visual Studio Developer Command Prompt:

  vcpkg install qtbase[core,gui,widgets]:x64-windows

Qt build may take significant amount of time

### Build remote-desktop-lab project

Replace all <vcpkg_path> entries in CMakePresets.json by correct path to vcpkg

Run build_release.cmd or build_debug.cmd

Wait until build is accomplished

Find executables in build\deploy directory