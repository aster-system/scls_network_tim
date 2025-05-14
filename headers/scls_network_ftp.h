//******************
//
// scls_network_ftp.h
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
// This file contains tools to use the FTP protocol.
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

#ifndef SCLS_NETWORK_FTP
#define SCLS_NETWORK_FTP

// Include the needed headers
#include "scls_network_socket.h"

// The namespace "scls" is used to simplify the all.
namespace scls {

    //*********
    //
    // FTP Client class
    //
    //*********

    class FTP_Client {
        // Class representating a FTP client
    public:
        // FTP_Client constructor
        FTP_Client(std::string ip):a_ip(ip),a_socket(ip, 21){};
        // FTP_Client destructor
        ~FTP_Client(){a_socket.close();};

        // Connects the client
        int connect(){
            // Create and connect a SOCKET for the server
            int error = a_socket.connect();
            if(error == -1){print(std::string("FTP Client \"") + a_ip + std::string("\""), std::string("Unable to create a socket"));return -1;}
            if(error == -2){print(std::string("FTP Client \"") + a_ip + std::string("\""), std::string("Unable to connect to the server"));return -2;}

            // Send the authentification request
            Server_Response response;
            // Get the DTP IP
            error = ftp_request(&a_socket, "USER anonymous\r\nPASS astersystemeoff@gmail.com\r\nMODE binary", &response);if(error < 0){return error;}
            error = ftp_request(&a_socket, std::string("EPSV\r\n"), &response);if(error < 0){return error;}
            std::vector<std::string> cutted = scls::cut_string(response.datas.get()->extract_string_all(), std::string("|"));
            if(cutted.size() > 2){a_dtp_port = std::stoi(cutted.at(cutted.size() - 2));}

            // Return a success
            return 0;
        };
        // Disconnect the client
        void disconnect(){Server_Response response;ftp_request(&a_socket, "QUIT\r\n", &response);}
        // Retrieve a file from the server
        Server_Response retrieve(std::string path){
            // Connect to DTP
            Server_Response response;
            ftp_request_no_receive(&a_socket, std::string("RETR ") + path + std::string("\r\n"), &response);
            response = send_request(a_ip, a_dtp_port, std::string());
            Server_Response final_response;ftp_request_no_noop(&a_socket, std::string(), &final_response);
            return response;
        }

        // Sends an FTP request
        int ftp_request(Socket* needed_socket, std::string request, Server_Response* response);
        int ftp_request_no_noop(Socket* needed_socket, std::string request, Server_Response* response);
        int ftp_request_no_receive(Socket* needed_socket, std::string request, Server_Response* response);

    private:
        // Address informations
        struct addrinfo* a_address_informations = 0;
        // DTP port of the FTP server
        int a_dtp_port = -1;
        // IP of the FTP server
        std::string a_ip;
        // Socket used
        Socket a_socket;
    };
}

#endif // SCLS_NETWORK_FTP
