// C++ program to illustrate the client application in the
// socket programming
#include <string>
#include <iostream>
#include "headers/scls_network_socket.h"

#define DEFAULT_BUFLEN 512

SCLS_INIT

int main() {
    // Create a request
    std::string address = std::string("aster-system.github.io");
    std::string request = "GET saasf/ HTTP/1.1\r\n";
    request += "Host: " + address + "\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";

    // Create a socket
    scls::Socket socket = scls::Socket(address, 80);
    socket.connect();
    socket.send_datas(request);
    std::cout << "E " << socket.receive_datas().datas.get()->extract_string_all() << std::endl;
    socket.close_socket();

    return 0;
}
