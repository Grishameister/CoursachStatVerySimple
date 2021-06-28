#ifndef HIGHLOAD_SERVER_SERVERSOCKET_H
#define HIGHLOAD_SERVER_SERVERSOCKET_H

#include <string>

class ServerSocket{
public:
    ServerSocket(const std::string& host, uint32_t port);

    bool SetNonBlocking(bool option);
    int GetFd() {
        return sock_fd_;
    }

    ~ServerSocket();
private:
    int sock_fd_;
};

#endif //HIGHLOAD_SERVER_SERVERSOCKET_H
