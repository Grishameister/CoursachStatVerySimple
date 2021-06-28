#ifndef HIGHLOAD_SERVER_WORKER_H
#define HIGHLOAD_SERVER_WORKER_H

#include <atomic>

#include "ConnectStructs.h"
#include "SafeQueue.h"
#include "Handlers.h"


class Worker {
public:
    Worker(SafeQueue<std::unique_ptr<Connection>>& q,
           const std::string &connection,
           SafeQueue<Status>& n_q,
           StatusSetter& setter) :
    m_q(q),
    hm(connection, n_q, setter) {}

    void work();

private:
    SafeQueue<std::unique_ptr<Connection>> &m_q;
    HandlerManager hm;
    static std::atomic<int> counter;
};

#endif //HIGHLOAD_SERVER_WORKER_H
