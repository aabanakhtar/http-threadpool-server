# HTTP Threadpool Server

A multithreaded C++ HTTP Server written with the POSIX API that supports serving static files for HTTP/1.1 that can be configured kinda like Apache. It uses a threadpool architecture to distribute the load of requests among processor cores.

It works by having the main thread block on `accept()`calls, upon which it offloads the handling of a request to a worker in the threadpool. The threadpool parses the request and attempts to serve a static file, as defined by the `content_directory`, error page, or predefined error page.

Using `wrk`for benchmarks on 4 cores and 600 concurrent requests for 15s on `index.html` resulted in the following stats on my Intel i7-9700F:

* Throughput: 63,647 Requests / Second
* Average Request Latency (all threads) : 8.4ms with standard deviation of +/- 2.17ms
* MB per sec transferred (all threads): 224.16MB / Second

## Features

* HTTP Request Parsing and Response Generation
* HTTP GET for HTML, CSS, Text, and PNG Images
* A Custom Threadpool Implementation using C++ Standard Library Threads, `condition_variable`, mutexes, etc.
* Configurable base directories and directory redirection attack prevention using `std::filesystem::weakly_canonical`
* Implementation of `404, 403, 500, 200, 415,` and `400` response codes
* Uses `sendfile() `internally to prevent copying of files between user and kernel space, increasing performance
* Modern C++ use (`std::variant, std::unordered_map,` etc.)

## **Building and Running**:

All you need is a POSIX compliant system (Linux is tested) and a C++23 Compiler w/ CMake:

```bash
$ mkdir build
cd build
cmake -G .. 
cmake --build . 
./http-threadpool-server
```

## High Level Overview

Configuration in main.cpp file:

```c++
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
```
