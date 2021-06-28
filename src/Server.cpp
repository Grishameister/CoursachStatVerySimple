#include <csignal>
#include <event2/event.h>
#include <iostream>

#include "Notifier.h"
#include "Server.h"
#include "Worker.h"

namespace {
    void on_read(int fd, short event, void* arg) {
        auto ptr = reinterpret_cast<ConnectionData*>(arg);
        if (!ptr) {
            return;
        }
        ptr->m_q.Push(std::make_unique<Connection>(fd));
        delete ptr;
    }

    void on_connect(int fd, short event, void *arg) {
        sockaddr_in client_addr;
        socklen_t   len = 0;

        int sock = accept(fd, reinterpret_cast<sockaddr*>(&client_addr), &len);
        if (sock < 1) {
            return;
        }

        auto ptr = reinterpret_cast<InAcceptConnection*>(arg);
        if (ptr == nullptr) {
            return;
        }

        ConnectionData* data = nullptr;
        try {
            data = new ConnectionData(sock, ptr->m_q);
        } catch (std::exception& e) {
            event_add(ptr->ev, nullptr);
            return;
        }

        data->ev = event_new(ptr->base, sock, EV_READ, on_read, data);
        if (data->ev == nullptr) {
            event_add(ptr->ev, nullptr);
            return;
        }

        event_add(data->ev, nullptr);
        event_add(ptr->ev, nullptr);
    }

    void sighandler(int signal, short events, void *arg)
    {
        event_base_loopbreak(reinterpret_cast<event_base*>(arg));
    }

    void add_sig_events(event_base* base) {
        auto evint = event_new(base, SIGINT, EV_SIGNAL | EV_PERSIST, sighandler, base);
        if (evint == nullptr) {
            return;
        }
        event_add(evint, nullptr);

        auto evterm = event_new(base, SIGTERM, EV_SIGNAL | EV_PERSIST, sighandler, base);
        if (evterm == nullptr) {
            return;
        }
        event_add(evterm, nullptr);
    }

}

void Server::run() {
    auto base = event_base_new();
    if (base == nullptr) {
        std::cout << "cannot init base of event loop" << std::endl;
        return;
    }

    auto conn_ptr = std::make_unique<InAcceptConnection>(m_sock_.GetFd(), GetQueueRef());

    conn_ptr->base = base;
    conn_ptr->ev = event_new(base, m_sock_.GetFd(), EV_READ, on_connect, conn_ptr.get());

    if (conn_ptr->ev == nullptr) {
        std::cout << "cannot allocate first event" << std::endl;
        return;
    }
    add_sig_events(base);
    event_add(conn_ptr->ev, nullptr);

    RunThreads();
    RunNotifier();

    event_base_dispatch(base);

    StopThreads();
    StopNotifications();
}

void Server::RunThreads() {
    for (size_t i = 0; i < m_max_threads; i++) {
        std::cout << i << std::endl;
        std::unique_ptr<Worker> w = std::make_unique<Worker>(GetQueueRef(), m_connection, n_q, setter);
        workers.emplace_back(std::thread([ptr = std::move(w)]() mutable {
            ptr->work();
        }));
    }
}

void Server::RunNotifier() {
    Notifier n(n_q);
    notifier = std::thread([n]() mutable {
        n.Run();
    });
}

void Server::StopThreads() {
    for (size_t i = 0; i < m_max_threads; i++) {
        m_q.Push(nullptr);
    }
    for (auto & w: workers) {
        w.join();
    }
}

void Server::StopNotifications() {
    Status st;
    n_q.Push(st);
    notifier.join();
}
