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

