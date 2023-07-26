#include <iostream>
#include <string>
#include <vcruntime.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <future>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Requester {
public:
    Requester(const std::string& channel = "5556") : channel(channel) {}

    void connect() {
        context = zmq::context_t{};
        socket = zmq::socket_t{context, zmq::socket_type::req};
        socket.connect("tcp://127.0.0.1:" + channel);
    }

    std::future<json> request(const json& msg) {
        return std::async(std::launch::async, [this, msg]() -> json {
            try {
                // Serialize JSON message to a string
                std::string msg_str = msg.dump();

                zmq::message_t request_msg(msg_str.size());
                memcpy(request_msg.data(), msg_str.data(), msg_str.size());

                socket.send(request_msg, zmq::send_flags::none);

                zmq::message_t response_msg;
                socket.recv(response_msg, zmq::recv_flags::none);

                // Deserialize the received JSON response
                std::string response_str(static_cast<char*>(response_msg.data()), response_msg.size());
                return json::parse(response_str);
            }
            catch (const zmq::error_t& e) {
                std::cout << "[Requester Error] " << e.what() << " Request: " << msg << std::endl;
                return json();
            }
            catch (const std::exception& e) {
                std::cout << "[Requester Error] " << e.what() << " Request: " << msg << std::endl;
                return json();
            }
        });
    }

    void close() {
        socket.close();
        context.close();
    }

private:
    std::string channel;
    zmq::context_t context;
    zmq::socket_t socket;
};

class Responder {
public:
    Responder(const std::string& channel = "5556") : channel(channel) {}

    void connect() {
        context = zmq::context_t{1};
        socket = zmq::socket_t{context, zmq::socket_type::rep};
        socket.bind("tcp://127.0.0.1:" + channel);
    }

    std::future<std::string> respond(const std::function<std::string(const std::string&)>& callback) {
        return std::async(std::launch::async, [this, callback]() -> std::string {
            try {
                zmq::message_t request_msg;
                socket.recv(request_msg, zmq::recv_flags::none);

                std::string msg(static_cast<char*>(request_msg.data()), request_msg.size());
                std::string response = callback(msg);

                zmq::message_t response_msg(response.size());
                memcpy(response_msg.data(), response.c_str(), response.size());

                socket.send(response_msg, zmq::send_flags::none);

                return response;
            }
            catch (const zmq::error_t& e) {
                std::cout << "[Response Error] " << e.what() << " Request: " << std::endl;
                return "";
            }
        });
    }

private:
    std::string channel;
    zmq::context_t context;
    zmq::socket_t socket;
};
