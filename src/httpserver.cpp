#include "httpserver.h" 

#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <filesystem>

#include "util.h"

constexpr static int BAD_FD = -1;



HttpServer::HttpServer(std::uint16_t port)
    : port(port), thread_pool() {}

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
            && util::checkUnixCall(setsockopt(connection_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)), "setsockopt"))) {
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
    std::osyncstream(std::cout) << "Started listening on port 8080!\n"; 

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


void HttpServer::handleRequestTask(int client) {
    std::string content = "<h1>hello</h1>";
    HttpResponse ok_200 {
        .response_code = ResponseCode::OK, 
        .content_type = ContentType::HTML,
        .body = content
    };
    
    int client_fd = client;
    //  char buffer[BUF_SIZE + 1] = {0};

    std::string request_str = "";
    constexpr static int BUF_SIZE = 4096;
    request_str.reserve(BUF_SIZE * 2);  

    while (true) {
        
        char buffer[BUF_SIZE + 1];
        ssize_t bytes_read = recv(client_fd, buffer, BUF_SIZE, 0); 

        if (bytes_read > 0) {
            request_str.append(buffer, bytes_read); // add to request
        }     
        
        // client disconnect / delimeter found / error
        // EINTR is an error in which "signals" briefly interrupt a task from continuing and is recoverable.
        if (request_str.contains("\r\n\r\n") || bytes_read == 0 || bytes_read < 0 && errno != EINTR) {
            break;
        }
    }

    HttpRequest request(request_str);
    dispatchResponse(client_fd, request);

    // send and ensure send completes before close
    shutdown(client_fd, SHUT_WR);
    close(client_fd);

}


void HttpServer::dispatchResponse(const int client, const HttpRequest& req) {
    HttpResponse response; 

    // generate a rsponse based on what the client requests based on method
    switch (req.method) {
        case RequestMethod::GET:
            httpGet(req, response);
            break;
        default: 
            response = HttpResponse {
                .response_code = ResponseCode::BAD_REQUEST, 
                .content_type = ContentType::TEXT,
                .body = "400, bad request" 
            };
            break;
    }

    // construct the response and send it over
    std::string response_str = response.constructResponse();
    send(client, response_str.c_str(), response_str.size(), 0); //MSG_ZEROCOPY?; 
}

void HttpServer::httpGet(const HttpRequest& req, HttpResponse& response) {
    // go the the default webpage, i.e, the index html
    // build the working directory for public content
    // and the index page
    static const std::string content_root = std::string(std::filesystem::current_path()) + content_directory;
    static const std::string directory_index_path = content_root + directory_index.path;

    // case 1: we get a request for the default page, check if it exists
    if (req.resource_uri == "/" && std::filesystem::exists(directory_index_path)) {
        std::ifstream file(directory_index_path);

        response = HttpResponse {
            .response_code = ResponseCode::OK, 
            .content_type = directory_index.type, 
            .body = std::string(std::istreambuf_iterator<char>{file}, {}) 
        };
        
        file.close();
    }
    // otherwise now just return 404 
    else {
        // check for user bound error page
        if (error_pages.contains(ResponseCode::NOT_FOUND)) {
            // load the user defined page
            HttpPage user_page = error_pages[ResponseCode::NOT_FOUND]; 

            // create the file
            std::string page_path = content_root + user_page.path;
            std::ifstream user_404_page_file(page_path);

            response = HttpResponse {
                .response_code = ResponseCode::NOT_FOUND, 
                .content_type = user_page.type, 
                .body = std::string(std::istreambuf_iterator<char>{user_404_page_file}, {}) 
            };

        }
        else {
            // just use a basic one 
            response = HttpResponse {
                .response_code = ResponseCode::NOT_FOUND, 
                .content_type = ContentType::TEXT,
                .body = "404 not found."
            };
        }
        
    }
}