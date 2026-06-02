#include <iostream> 
#include <chrono> 
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>

#include "util.h"
#include "httpserver.h"
#include <signal.h>

int main(int argc, char** argv) {
    // prevent closed connections from yeeting us into the unknown
    signal(SIGPIPE, SIG_IGN);

    HttpServer service(8080);

    service.setContentDirectory("/www/");
    service.setDirectoryIndex(HttpPage{
        .type = ContentType::HTML,
        .path = "index.html",
    });

    service.createErrorPageBinding(ResponseCode::NOT_FOUND, HttpPage{
        .type = ContentType::HTML,
        .path = "404.html", 
    });

    service.createErrorPageBinding(ResponseCode::FORBIDDEN, HttpPage{
        .type = ContentType::HTML,
        .path = "403.html"
    });

    if (service.initialize()) {
        service.startListening();
        return 0;
    }


    return 1;
}