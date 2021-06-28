#ifndef HIGHLOAD_SERVER_CONNECTSTRUCTS_H
#define HIGHLOAD_SERVER_CONNECTSTRUCTS_H

#include <event.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "SafeQueue.h"

struct Connection {
    int m_fd = -1;
    explicit Connection(int fd) : m_fd(fd) {}

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
            ssize_t bytes_read = read(m_fd, buf, chunk_size);

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
            ssize_t  bytes_wrote = write(m_fd, data, length);
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
public:
    ~Connection() {
        if (m_fd != -1) {
            ::close(m_fd);
        }
    }
};

struct ConnectionData {
    SafeQueue<std::unique_ptr<Connection>>& m_q;
    struct event* ev;
    int m_fd;

    ConnectionData(int fd, SafeQueue<std::unique_ptr<Connection>>& q): m_fd(fd), m_q(q), ev(nullptr) {}
    virtual ~ConnectionData() {
        if (ev != nullptr) {
            event_free(ev);
        }
    }
};

struct InAcceptConnection: public ConnectionData {
    struct event_base* base;

    InAcceptConnection(int fd, SafeQueue<std::unique_ptr<Connection>>& q): ConnectionData(fd, q), base(nullptr) {}
    ~InAcceptConnection() {
        if (base != nullptr) {
            event_base_free(base);
        }
    }
};

#endif //HIGHLOAD_SERVER_CONNECTSTRUCTS_H
