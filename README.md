# HTTP Threadpool Server

A custom C++ HTTP Server written with the POSIX API that supports serving static files for HTTP/1.1 that can be configured kinda like Apache. It uses a threadpool architecture to distribute the load of requests among processor cores. 


# Features

* HTTP Request Parsing and Response Generation
* HTTP GET for HTML, CSS, Text, and PNG Images
* A Custom Threadpool Implementation using C++ Standard Library Threads, `condition_variable`, mutexes, etc.
* Configurable base directories and directory redirection attack prevention using `std::filesystem::canonical`
* Implementation of `404, 403, 500, 200, 415,` and `400` response codes
* Uses `sendfile() `internally to prevent copying of files between user and kernel space, increasing performance
* Modern C++ use (`std::variant, std::unordered_map,` etc.)
* YAML configuration
