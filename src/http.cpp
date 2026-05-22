#include "http.h" 

#include <arpa/inet.h>
#include <unistd.h>

#include "util.h"

constexpr static int BAD_FD = -1;

HttpServer::HttpServer(std::uint16_t port)
    : port{port} {}

HttpServer::~HttpServer() {
    if (connection_fd == BAD_FD) {
        return;
    }

    close(connection_fd);
    connection_fd = BAD_FD;
}

bool HttpServer::initialize() {
    // create socket
    if(auto fd = util::checkUnixCall(socket(AF_INET, SOCK_STREAM, 0), "socket")) {
        connection_fd = *fd;
    } else {
        connection_fd = BAD_FD;
        return false; 
    }

    int enable = 1; 
    // make ip reusable instantly so theres no cool-off period  
    if (!(util::checkUnixCall(setsockopt(connection_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)), "setsockopt")
            && util::checkUnixCall(setsockopt(connection_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)), "setsockopt"))) {
        goto failure;
    }

    // setup ip
    sockaddr_in ip;
    ip.sin_family = AF_INET; // creates an internet ip 
    ip.sin_port = htons(port); // binds the port 
    ip.sin_addr.s_addr = INADDR_ANY; // binds to all addresses assigned to this machine as long as port = 8080 

    // forces the ip onto the socket
    if (!util::checkUnixCall(bind(connection_fd, ((sockaddr*)&ip), sizeof(ip)), "bind")) {
        goto failure; 
    }

    // allows maximum incoming requests for connections, starts listening on this fd
    if (!util::checkUnixCall(listen(connection_fd, SOMAXCONN), "listen")) {
        goto failure; 
    }

    return true;

failure:
    // clean up from failed operation
    close(connection_fd);
    connection_fd = BAD_FD;
    return false; 
}

void HttpServer::startListening() {
    static std::string http_200_ok = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 122\r\n" // Updated to match the actual body length below
    "\r\n"                    // CRITICAL: Empty line separating headers from body
    "<!doctype html>\n"
    "<html>\n"
    "<head><title>Example</title></head>\n"
    "<body>Welcome to Example.com</body>\n"
    "</html>";

    while (true) {
        sockaddr_in client_ip_info;
        socklen_t size = sizeof(client_ip_info); 

        if (auto client_fd = util::checkUnixCall(accept(connection_fd, (sockaddr*)&client_ip_info, &size), "accept")) {
            send(*client_fd, http_200_ok.c_str(), http_200_ok.length(), 0);
            close(*client_fd);
        }
    }
    
}

