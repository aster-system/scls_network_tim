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

// Socket system
#if defined(__WIN32__) || defined(__WIN64__) // With Windows
// Include the good header file
extern "C" {
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <stdio.h>
}
#endif
#ifdef __linux__ // With Linux
// Include the good header file
extern "C" {
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <stdio.h>
}
#endif

#include "../../scls-foundation-leonhard/scls_foundation.h"
#include <thread>

// The namespace "scls" is used to simplify the all.
namespace scls {

    #if defined(__WIN32__) || defined(__WIN64__) // With Windows
    // Gets informations about an address
    int address_informations(struct addrinfo **informations, std::string ip, int port);

    // Connects a socket
    void connect_socket(SOCKET& needed_socket, struct addrinfo* informations);

    // Creates a socket
    SOCKET create_socket(struct addrinfo* informations);

    // Sends a request to a server with an existing socket
    int send_request_with_socket(SOCKET* needed_socket, std::string request, Server_Response* response);

    // Sends a request to a server and returns the response
    Server_Response send_request(std::string ip, int port, std::string request);
    int send_request_asynchronous(std::string ip, int port, std::string request, Server_Response* response, bool* validation, std::thread* used_thread);

    // Initializes Winsock
    int start_wsa();

    // Get Winsock
    WSADATA& wsa_data();
    #endif
    #ifdef __linux__ // With Linux
    #endif

    //*********
    //
    // The Socket class
    //
    //*********

    // Function to start the network
    extern bool __network_started;
    inline int start_network(){
        // Assets
        if(__network_started){return 0;}

        // With Windows
        #if defined(__WIN32__) || defined(__WIN64__)
        return start_wsa();
        #endif
    };

    // Response of a server
    struct Server_Response{std::shared_ptr<Bytes_Set> datas;};

    class Socket {
        // Class representating a network socket
    public:
        // Socket constructor
        Socket(std::string ip, int port):a_ip(ip),a_port(port){start_network();__address_informations();};

        // Closes the socket
        int close(){
            // Asserts
            if(!a_opened){return 0;}
            a_opened = false;

            // With Windows
            #if defined(__WIN32__) || defined(__WIN64__)
            return closesocket(a_socket_handle);
            #endif

            // With Linux
            #ifdef __linux__
            return close(a_socket_handle);
            #endif
        }

        // Connects the socket
        #if defined(__WIN32__) || defined(__WIN64__) // With Windows
        void __connect_windows();
        #endif
        #ifdef __linux__ // With Linux
        void __connect_linux();
        #endif
        void connect(){
            #if defined(__WIN32__) || defined(__WIN64__) // With Windows
            __connect_windows();
            #endif
            #ifdef __linux__ // With Linux
            __connect_linux();
            #endif
        };

        // Gets the informations about the address
        #if defined(__WIN32__) || defined(__WIN64__) // With Windows
        int __address_informations_windows();
        #endif
        #ifdef __linux__ // With Linux
        int __address_informations_linux();
        #endif
        int __address_informations(){
            #if defined(__WIN32__) || defined(__WIN64__) // With Windows
            return __address_informations_windows();
            #endif
            #ifdef __linux__ // With Linux
            return __address_informations_linux();
            #endif
        };

    private:

        // Set the network datas
        // IP of the server
        std::string a_ip = std::string();
        // If the socket is opened or not
        bool a_opened = false;
        // Port of the server
        int a_port = -1;

        // Set the good socket datas
        #if defined(__WIN32__) || defined(__WIN64__) // With Windows
        SOCKET a_socket_handle;
        #endif
        #ifdef __linux__ // With Linux
        struct sockaddr_in a_address_informations;
        struct hostent* a_server;
        int a_socket_handle;
        #endif
    };
}

#endif // SCLS_NETWORK_SOCKET
