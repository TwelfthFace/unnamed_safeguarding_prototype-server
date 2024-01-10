# Unnamed Safeguarding Prototype Server

## Overview
This server application is part of the Unnamed Safeguarding Prototype project. It is designed to manage client connections securely, handle data transmission, and provide real-time monitoring features.

## Features
- **Secure Client-Server Communication**: Utilizes SSL/TLS for encrypted communication between the server and clients.
- **Real-Time Monitoring**: Supports real-time data transmission, including screenshots and text data from clients.
- **Client Management**: Efficient handling of multiple client connections and data streams.

## Getting Started

### Prerequisites
- Boost.Asio for network programming.
- OpenSSL for SSL/TLS support.
- Qt for the graphical user interface components.

### Installation
1. Clone the repository.
2. Ensure all dependencies are installed.
3. Compile the project using the provided `.pro` file with Qt.

### Usage
- Run the server application.
- Connect clients to the server.
- Monitor and manage client data through the server interface.

## Code Structure
- `Server.cpp/h`: Core server functionality including SSL context setup and client connection management.
- `ClientConnection.cpp/h`: Handles individual client connections and data transmission.
- `mainwindow.cpp/h`: Main window of the server's GUI.
- `dialog.cpp/h`: Dialog windows for additional client interaction.
- `my_grid_layout.cpp/h`: Custom layout manager for the GUI.

## Acknowledgements
- Boost.Asio library
- OpenSSL
- Qt framework
