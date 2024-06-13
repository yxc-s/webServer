# WebServer

## Introduction
This project was initiated in February 2024 with the aim of learning about network communications. It involves writing a network framework in C++ to apply knowledge gained in practical scenarios. The design philosophy of the framework is based on a high-performance C++ server framework utilizing sockets, combined with a bridge for handling communications between HTTP and the socket framework internally, and a basic HTTP framework. The server handles network connections, while the bridge routes requests to the HTTP module for parsing and response.

## Technology Stack
- C++17
- Thread pool
- Min-heap timers
- HTTP data parsing and response
- RAII (Resource Acquisition Is Initialization)

## Usage
1. In a Linux environment, navigate to the `build` directory and run the following command in the terminal: cmake..
2. Navigate to the `bin` folder and execute: cmake hpts
3. To start the server, run: ./hpts
4. Access the server using a browser at `ip:port`.
5. IP address and port can be configured in `./config/socketbaseconfig.ini`.
6. Place static resources in `./static/pages`.

## TODO
- Implement a logging system.
- Integrate a database.

Thank you for exploring this project, and I hope it serves as a valuable tool for learning and application in network programming.
