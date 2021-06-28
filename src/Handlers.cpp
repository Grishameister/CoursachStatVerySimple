#include "Handlers.h"

#include <iostream>

bool ReadHandler::CanHandle(const DataRequest &req) {
    return req.type == Route::ReadStatData;
}

DataResponse ReadHandler::Handle(const DataRequest& req) {
    Counters date_counters = connection->ReadData(req.date);
    DataResponse result;
    result.date = req.date;
    if (date_counters.HasEmptyFields()) {
        result.error = Error::NotFound;
        return result;
    }
    result.error = Error::Success;
    result.counters = date_counters;
    return result;
}


bool WriteStatHandler::CanHandle(const DataRequest &req) {
    return req.type == Route::WriteStateData;
}

DataResponse WriteStatHandler::Handle(const DataRequest &req) {
    connection->WriteData(req.date, req.counters);

    std::string status = GetStatus(req.counters);
    if (m_setter.SetStatus(status)) {
        Status st;
        st.status = status;
        st.date = req.date;
        std::cout << status << std::endl;
        notifications_q.Push(st);
    }

    return DataResponse{};
}

std::string WriteStatHandler::GetStatus(const Counters& c) {
    std::string result;
    if (c.people != counters.people) {
        result.append("NotEnoughPeople,");
    }
    if (c.buldozers != counters.buldozers){
        result.append("NotEnoughBuldozers,");
    }
    if (c.excavators != counters.excavators) {
        result.append("NotEnoughExcavators");
    }
    if (result.empty()) {
        result.append("OK");
    }
    return result;
}

std::string HandlerManager::Handle(const std::string& message) {
    DataResponse resp;
    std::unique_ptr<DataRequest> req;
    try {
        req = p.Deserialize<DataRequest>(message);
    } catch (...) {
        return "data deserialization error occurredt";
    }

    for (auto& h: handlers) {
        if (h->CanHandle(*req)) {
            resp = h->Handle(*req);
            break;
        }
    }

    return p.Serialize(resp);
}
