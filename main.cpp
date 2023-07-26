#include <iostream>
#include "Messaging.hpp"
#include <string>
#include <future>
#include <chrono>

int main() {
    std::cout << "Hello, World!\n";
    Requester requester;
    requester.connect();

    json request_msg = {
        {"hello", "server!"}
    };

    // Using the request method as an async operation
    auto response_future = requester.request(request_msg);

    json response = response_future.get();

    std::string name = response["hello"];

    std::cout << "Response from server: " << std::endl;
    std::cout << "Hello: " << name << std::endl;

    requester.close();

    return 0;
}
