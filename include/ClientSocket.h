#ifndef HIGHLOAD_SERVER_CLIENTSOCKET_H
#define HIGHLOAD_SERVER_CLIENTSOCKET_H

#include <cstring>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

class ClientSocket {
public:
    ClientSocket(const std::string& host, uint32_t port) {
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

        if (connect(sock_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw std::runtime_error("sockopt: " + std::string(strerror(errno)));
        }
    }

    ~ClientSocket() {
        ::close(sock_fd_);
    }

    std::string ReadAllBytes(size_t limit) {
        std::ostringstream oss;
        while (oss.tellp() < limit) {
            std::string s_buf(1024, '\0');
            size_t bytes_read = readBytes(s_buf.data(), s_buf.size());

            oss.write(s_buf.c_str(), static_cast<long>(bytes_read));

            if (bytes_read < s_buf.size()) {
                break;
            }
        }
        return oss.str();
    }

    void WriteAllBytes(const void *data, size_t length) const {
        size_t bytes_wrote = 0;
        while (bytes_wrote != length) {
            auto begin_of_buffer = static_cast<const char*>(data) + bytes_wrote;
            size_t bytes = WriteBytes(begin_of_buffer, length - bytes_wrote);
            bytes_wrote += bytes;
        }
    }

private:
    size_t readBytes(void* buf, size_t chunk_size) {
        while(true) {
            ssize_t bytes_read = read(sock_fd_, buf, chunk_size);

            if (bytes_read < 0) {
                if (errno == EINTR) {
                    std::cout << "Interrupted system call" <<std::endl;
                    continue;
                }
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    std::cout << "Operation would block" <<std::endl;
                    return 0;
                }
            }
            return static_cast<size_t>(bytes_read);
        }
    }

    size_t WriteBytes(const void *data, size_t length) const {
        while(true) {
            ssize_t  bytes_wrote = write(sock_fd_, data, length);
            if (bytes_wrote < 0) {
                if (errno == EINTR) {
                    continue;
                }
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return 0;
                }
                std::cout << "wtf" << std::endl;
            }
            return static_cast<size_t>(bytes_wrote);
        }
    }
    int sock_fd_;
};

#endif //HIGHLOAD_SERVER_CLIENTSOCKET_H
