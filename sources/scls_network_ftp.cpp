//******************
//
// scls_network_ftp.cpp
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
// This file contains the source code of scls_network_ftp.h.
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

// Include the needed headers
#include "../headers/scls_network_ftp.h"

// The namespace "scls" is used to simplify the all.
namespace scls {

    //*********
    //
    // FTP Client class
    //
    //*********

    // Sends an FTP request
    int FTP_Client::ftp_request(SOCKET* needed_socket, std::string request, Server_Response* response) {
        // Parse the request
        if(request.substr(request.size() - 6, 6) != std::string("NOOP\r\n") && request.substr(request.size() - 6, 6) != std::string("QUIT\r\n")){request = request + std::string("NOOP\r\n");}
        return ftp_request_no_noop(needed_socket, request, response);
    }
    int FTP_Client::ftp_request_no_noop(SOCKET* needed_socket, std::string request, Server_Response* response) {
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
        int result = 0;
        do {
            result = recv(*needed_socket, recvbuf, recvbuflen, 0);
            if (result > 0){datas_size += result;datas.push_back(std::make_shared<Bytes_Set>());datas.at(datas.size() - 1).get()->add_datas(recvbuf, result);}
            else if (result < 0){return -2;}

            if(scls::contains_string(datas.at(datas.size() - 1).get()->extract_string_all(), std::string("NOOP"))){break;}
        }
        while (result > 0);

        // Get the final datas
        std::shared_ptr<Bytes_Set>& final_datas = response->datas;
        final_datas = std::make_shared<Bytes_Set>(datas_size);int pos = 0;
        for(int i = 0;i<static_cast<int>(datas.size());i++){final_datas.get()->put_datas(datas.at(i).get(), pos);pos += datas.at(i)->datas_size();}
        return 0;
    }
    int FTP_Client::ftp_request_no_receive(SOCKET* needed_socket, std::string request, Server_Response* response) {
        // Parse the request
        if(request.size() < 6 || (request.substr(request.size() - 6, 6) != std::string("NOOP\r\n") && request.substr(request.size() - 6, 6) != std::string("QUIT\r\n"))){request = request + std::string("NOOP\r\n");}

        // Send the request
        int iResult = send(*needed_socket, request.c_str(), static_cast<int>(request.size()), 0 );
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(*needed_socket);
            return -1;
        }

        // Returns the result
        return 0;
    }
}
