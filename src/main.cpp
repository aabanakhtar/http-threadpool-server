#include <iostream> 
#include <chrono> 
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std::chrono_literals;

int main(int argc, char** argv) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    close(socket_fd);  
    return 0;
}