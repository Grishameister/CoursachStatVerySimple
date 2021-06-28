#include <iostream>

#include "Worker.h"
#include "Data.h"

std::atomic<int> Worker::counter{0};

void Worker::work() {
    bool can_exit = false;
    while (!can_exit) {
        std::unique_ptr<Connection> c;
        m_q.WaitPop(c);
        if (c == nullptr) {
            can_exit = true;
            continue;
        }

        std::string buf = c->ReadAllBytes(4096);
        std::string response = hm.Handle(buf);
        c->WriteAllBytes(response.data(), response.size());
    }
}

