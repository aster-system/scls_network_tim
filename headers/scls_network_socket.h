//******************
//
// scls_network_socket.h
//
//******************
// Presentation :
//
// SCLS is a project containing base functions for C++.
// It can also be use in any projects.
//
// The Network "Tim" part represents an interface to use network with SCLS.
// It is named after one of the founder of the Web : Tim Berners Lee.
//
// This file contains the "Socket" system, the base of the network system.
//
//******************
//
// License (LGPL V3.0) :
//
// Copyright (C) 2024 by Aster System, Inc. <https://aster-system.github.io/aster-system/>
// This file is part of SCLS.
// SCLS is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// SCLS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with SCLS. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef SCLS_NETWORK_SOCKET
#define SCLS_NETWORK_SOCKET

// Include the good header file
extern "C" {
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <stdio.h>
}
#include "../../scls-foundation-leonhard/scls_foundation.h"
#include <thread>

// The namespace "scls" is used to simplify the all.
namespace scls {

    // Gets informations about an address
    int address_informations(struct addrinfo **informations, std::string ip, int port);

    // Connects a socket
    void connect_socket(SOCKET& needed_socket, struct addrinfo* informations);

    // Creates a socket
    SOCKET create_socket(struct addrinfo* informations);

    // Sends a request to a server with an existing socket
    struct Server_Response{std::shared_ptr<Bytes_Set> datas;};
    int send_request_with_socket(SOCKET* needed_socket, std::string request, Server_Response* response);

    // Sends a request to a server and returns the response
    Server_Response send_request(std::string ip, int port, std::string request);
    int send_request_asynchronous(std::string ip, int port, std::string request, Server_Response* response, bool* validation, std::thread* used_thread);

    // Initializes Winsock
    int start_wsa();

    // Get Winsock
    WSADATA& wsa_data();
}

#endif // SCLS_NETWORK_SOCKET