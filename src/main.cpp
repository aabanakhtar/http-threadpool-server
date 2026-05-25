#include <iostream> 
#include <chrono> 
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>

#include "util.h"
#include "httpserver.h"

using namespace std::chrono_literals;

int main(int argc, char** argv) {
    HttpServer service(8080);

    if (service.initialize()) {
        service.startListening();
        return 0;
    }


    return 1;
}