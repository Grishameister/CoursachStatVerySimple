#ifndef HIGHLOAD_SERVER_SERVER_H
#define HIGHLOAD_SERVER_SERVER_H

#include <cstdio>
#include <string>
#include <unistd.h>
#include <thread>

#include "SafeQueue.h"
#include "Data.h"
#include "ServerSocket.h"
#include "ConnectStructs.h"
#include "StatusSetter.h"


class Server {
public:
    Server(std::size_t max_threads, const std::string& host, uint32_t port, const std::string& connection) :
            m_sock_(host, port), m_q(), m_max_threads(max_threads),
            m_connection(connection), workers(), n_q(), setter() {
    }

    SafeQueue<std::unique_ptr<Connection>>& GetQueueRef() {
        return m_q;
    }

    void run();
private:
    void RunThreads();
    void RunNotifier();
    void StopThreads();
    void StopNotifications();
    std::size_t m_max_threads;
    std::string m_connection;
    ServerSocket m_sock_;

    SafeQueue<Status> n_q;
    StatusSetter setter;
    SafeQueue<std::unique_ptr<Connection>> m_q;
    std::vector<std::thread> workers;
    std::thread notifier;
};

#endif //HIGHLOAD_SERVER_SERVER_H
