#include <iostream> 
#include <chrono> 
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>

#include "util.h"

using namespace std::chrono_literals;

int main(int argc, char** argv) {
    constexpr std::uint16_t port = 8080;

    int socket_fd = util::checkUnixCall(socket(AF_INET, SOCK_STREAM, 0), "socket", true);
    
    int enable = 1; 
    // set some options to make it reusable
    util::checkUnixCall(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)), "setsockopt", true);
    util::checkUnixCall(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)), "setsockopt", true); 

    // setup ip
    sockaddr_in ip;
    ip.sin_family = AF_INET; // creates an internet ip 
    ip.sin_port = htons(port); // binds the port 
    ip.sin_addr.s_addr = INADDR_ANY; // binds to all addresses assigned to this machine as long as port = 8080 

    // forces the ip onto the socket
    util::checkUnixCall(bind(socket_fd, ((sockaddr*)&ip), sizeof(ip)), "bind", true);
    // allows 3 maximum incoming requests for connections, starts listening on this fd
    util::checkUnixCall(listen(socket_fd, 3), "listen", true);

    socklen_t addrlen = sizeof(ip);
    int client_fd = util::checkUnixCall(accept(socket_fd, (sockaddr*)&ip, &addrlen), "accept", true);

    std::string http_200_ok = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 122\r\n" // Updated to match the actual body length below
    "\r\n"                    // CRITICAL: Empty line separating headers from body
    "<!doctype html>\n"
    "<html>\n"
    "<head><title>Example</title></head>\n"
    "<body>Welcome to Example.com</body>\n"
    "</html>";

    send(client_fd, http_200_ok.c_str(), http_200_ok.length(), 0);
    close(client_fd);
    close(socket_fd);  
    return 0;
}