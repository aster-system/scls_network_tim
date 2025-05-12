//******************
//
// scls_network_socket.cpp
//
//******************
// Presentation :
//
// SCLS is a project containing base functions for C++.
// It can also be use in any projects.
//
// The Image "Michelangelo" part represents a lot of usefull datas manipulation for images.
// It is named after one of the greatest painter of all time, Michelangelo.
//
// This file contains the source code of scls_network_socket.h.
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

// Include the good header file
#include "../headers/scls_network_socket.h"

#define DEFAULT_BUFLEN 512

// The namespace "scls" is used to simplify the all.
namespace scls {
    // Datas of the WSA server
    WSADATA __wsa_data;bool __wsa_started = false;

    // Gets informations about an address
    int address_informations(struct addrinfo **informations, std::string ip, int port) {
        // Create the address
        struct addrinfo hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Get the server address and port
        int result = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, informations);
        if (result != 0) {scls::print("WSA Server", std::string("Can't get datas about the address \"") + ip + std::string("\" : error ") + std::to_string(result));return -1;}
        return 0;
    }

    // Connects a socket
    void connect_socket(SOCKET& needed_socket, struct addrinfo* informations){int result = connect(needed_socket, informations->ai_addr, (int)informations->ai_addrlen);if (result == SOCKET_ERROR) {closesocket(needed_socket);needed_socket = INVALID_SOCKET;}}

    // Creates a socket
    SOCKET create_socket(struct addrinfo* informations) {
        SOCKET connect_socket = socket(informations->ai_family, informations->ai_socktype, informations->ai_protocol);
        if (connect_socket == INVALID_SOCKET) {scls::print("WSA Server", std::string("Can't create a socket"));return INVALID_SOCKET;}
        return connect_socket;
    }

    // Sends a request to a server with an existing socket
    int send_request_with_socket(SOCKET* needed_socket, std::string request, Server_Response* response) {
        // Send the request
        int iResult = send(*needed_socket, request.c_str(), static_cast<int>(request.size()), 0 );
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(*needed_socket);
            return -1;
        }

        // Receive data until the server closes the connection
        #define DEFAULT_BUFLEN 512
        int datas_size = 0;std::vector<std::shared_ptr<Bytes_Set>> datas;
        int recvbuflen = DEFAULT_BUFLEN;char recvbuf[DEFAULT_BUFLEN];
        scls::print("Network", "Start receiving datas...");
        int result = 0;
        do {
            result = recv(*needed_socket, recvbuf, recvbuflen, 0);
            if (result > 0){printf("Bytes received: %d\n", result);datas_size += result;datas.push_back(std::make_shared<Bytes_Set>());datas.at(datas.size() - 1).get()->add_datas(recvbuf, result);}
            else if (result == 0){printf("Connection closed\n");}
            else{printf("recv failed: %d\n", WSAGetLastError());}
        }
        while (result > 0);

        // Get the final datas
        std::shared_ptr<Bytes_Set>& final_datas = response->datas;
        final_datas = std::make_shared<Bytes_Set>(datas_size);int pos = 0;
        for(int i = 0;i<static_cast<int>(datas.size());i++){final_datas.get()->put_datas(datas.at(i).get(), pos);pos += datas.at(i)->datas_size();}
        return 0;
    }

    // Sends a request to a server and returns the response
    void __send_request(struct addrinfo *address, std::string ip, int port, std::string request, Server_Response* response, bool* validation){
        // Create and connect a SOCKET for the server
        int error = 0;
        SOCKET needed_socket = scls::create_socket(address);
        if(needed_socket == INVALID_SOCKET){printf("Unable to create a socket!\n");__wsa_started=false;WSACleanup();freeaddrinfo(address);error = -1;}
        if(error >= 0){
            scls::connect_socket(needed_socket, address);__wsa_started=false;freeaddrinfo(address);
            if (needed_socket == INVALID_SOCKET) {printf("Unable to connect to server!\n");WSACleanup();error = -2;}
        }

        // Send the request
        send_request_with_socket(&needed_socket, request, response);
        closesocket(needed_socket);
        (*validation) = true;
    }
    struct addrinfo* __temp_infos = 0;
    Server_Response send_request(std::string ip, int port, std::string request){
        // Get the address informations
        scls::start_wsa();
        struct addrinfo* infos = 0;
        int result = scls::address_informations(&infos, ip, port);
        if(result != 0){WSACleanup();__wsa_started=false;freeaddrinfo(infos);return Server_Response();}

        // Send the request
        bool validation = false;Server_Response response;
        __send_request(infos, ip, port, request, &response, &validation);
        return response;
    }
    int send_request_asynchronous(std::string ip, int port, std::string request, Server_Response* response, bool* validation, std::thread* used_thread) {
        // Get the address informations
        scls::start_wsa();
        int result = scls::address_informations(&__temp_infos, ip, port);
        if(result != 0){WSACleanup();__wsa_started=false;freeaddrinfo(__temp_infos);return 1;}

        // Asynchronaly send the request
        (*used_thread) = std::thread(__send_request, __temp_infos, ip, port, request, response, validation);
        return 0;
    }

    // Initializes Winsock
    int start_wsa() {if(__wsa_started){return 0;}int result = WSAStartup(MAKEWORD(2,2), &__wsa_data);if (result != 0) {scls::print("WSA Server", std::string("Can't open the server because of error ") + std::to_string(result));return 1;}__wsa_started=true;return 0;}

    // Get Winsock
    WSADATA& wsa_data(){start_wsa();return __wsa_data;}
}
