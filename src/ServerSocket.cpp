#include "ServerSocket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

ServerSocket::ServerSocket(const std::string& host, uint32_t port) {
    sock_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd_ <= 0) {
        throw std::runtime_error("socket: " + std::string(strerror(errno)));
    }

    int yes = 1;
    if (setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        throw std::runtime_error("sockopt: " + std::string(strerror(errno)));
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(host.c_str());
    server_addr.sin_port = htons(port);

    if (bind(sock_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        throw std::runtime_error("bind: " + std::string(strerror(errno)));
    }
    listen(GetFd(), 1000);
}

bool ServerSocket::SetNonBlocking(bool option) {
    int flags = fcntl(sock_fd_, F_GETFL, 0);
    int new_flags = (option)? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    return (fcntl(sock_fd_, F_SETFL, new_flags) == -1);
}

ServerSocket::~ServerSocket() {
    close(sock_fd_);
}
