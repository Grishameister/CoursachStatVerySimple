#ifndef HIGHLOAD_SERVER_HANDLERS_H
#define HIGHLOAD_SERVER_HANDLERS_H

#include <string>

#include "Data.h"
#include "IDBConnection.h"
#include "Parser.h"
#include "SafeQueue.h"
#include "StatusSetter.h"

class IHandler {
public:
    virtual DataResponse Handle(const DataRequest& req) = 0;

    virtual bool CanHandle(const DataRequest& req) = 0;
};

class ReadHandler : public IHandler {
public:
    ReadHandler(std::unique_ptr<IDBConnection> db_conn) : connection(std::move(db_conn)) {}

    bool CanHandle(const DataRequest& req) override;

    DataResponse Handle(const DataRequest& req) override;

private:
    std::unique_ptr<IDBConnection> connection;
};

class WriteStatHandler : public IHandler {
public:
    WriteStatHandler(std::unique_ptr<IDBConnection> db_conn, SafeQueue<Status>& q, StatusSetter& setter) :
    connection(std::move(db_conn)), counters(), notifications_q(q), m_setter(setter) {
        counters.people = 28;
        counters.buldozers = 5;
        counters.excavators = 8;
    }

    bool CanHandle(const DataRequest &req) override;

    DataResponse Handle(const DataRequest &req) override;

private:
    std::string GetStatus(const Counters& counters);
    Counters counters;
    StatusSetter& m_setter;
    SafeQueue<Status>& notifications_q;
    std::unique_ptr<IDBConnection> connection;
};

class HandlerManager {
private:
    std::vector<std::unique_ptr<IHandler>> handlers;
    Parser p;
public:
    HandlerManager(const std::string &connection, SafeQueue<Status>& n_q, StatusSetter& setter) :
        handlers(), p() {
        std::unique_ptr<IDBConnection> db_conn_read;
        std::unique_ptr<IDBConnection> db_conn_write;
        try {
            db_conn_read = std::make_unique<DBAccess>(connection);
            db_conn_write = std::make_unique<DBAccess>(connection);
        } catch (std::exception &e) {
            e.what();
            return;
        }

        handlers.emplace_back(std::make_unique<ReadHandler>(std::move(db_conn_read)));
        handlers.emplace_back(std::make_unique<WriteStatHandler>(std::move(db_conn_write), n_q, setter));
    }

    std::string Handle(const std::string &message);
};


#endif //HIGHLOAD_SERVER_HANDLERS_H
