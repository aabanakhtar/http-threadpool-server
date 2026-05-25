#include "httpserver.h" 

#include <arpa/inet.h>
#include <unistd.h>

#include "util.h"

constexpr static int BAD_FD = -1;



HttpServer::HttpServer(std::uint16_t port)
    : port(port), thread_pool(2) {}

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


    while (true) {
        sockaddr_in client_ip_info;
        socklen_t size = sizeof(client_ip_info); 

        if (auto client_fd = util::checkUnixCall(accept(connection_fd, (sockaddr*)&client_ip_info, &size), "accept")) {
            // offload to worker thread
            using namespace std::placeholders;
            thread_pool.postTask(Task{
                .task = std::bind(&HttpServer::handleRequestTask, this, _1),
                .data = *client_fd,
            }); 
        }
    }
    
}

static std::mutex cout_mutex;

void HttpServer::handleRequestTask(int data) {
    constexpr static std::size_t BUF_SIZE = 4096;

    std::string content = "<h1>hello</h1>";
    HttpResponse ok_200 {
        .response_code = ResponseCode::OK, 
        .content_type = ContentType::HTML,
        .body = content
    };
    
    int client_fd = data;
    char buffer[BUF_SIZE + 1] = {0}; 
    std::size_t bytes_read = recv(client_fd, buffer, BUF_SIZE, 0); 
    buffer[std::min(bytes_read, BUF_SIZE)] = 0;

    HttpRequest request(buffer);
    if (request.method != RequestMethod::BAD) {
        std::lock_guard lck(cout_mutex); 
        std::cout << request.resource_uri << std::endl; 
    }

    if (request.resource_uri == "/eat_mom") {
        HttpResponse mom_eaten = {
            .response_code = ResponseCode::OK, 
            .content_type = ContentType::TEXT, 
            .body = "Ur mom has been eaten! :O"
        }; 
        send(client_fd, mom_eaten.constructResponse().c_str(), mom_eaten.constructResponse().size(), 0);
    }
    else {
        // TODO: use zero copy to send files
        std::string response = ok_200.constructResponse();
        send(client_fd, response.c_str(), response.size(), 0);
    }

    // send and ensure send completes before close
    shutdown(client_fd, SHUT_WR);
    close(client_fd);

}
