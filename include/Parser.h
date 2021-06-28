#ifndef HIGHLOAD_SERVER_PARSER_H
#define HIGHLOAD_SERVER_PARSER_H

#include "Data.h"

class Parser {
public:
    template<typename T>
    std::unique_ptr<T> Deserialize(const std::string& request) {
        std::unique_ptr<T> rq = std::make_unique<DataRequest>();
        msgpack::object_handle oh = msgpack::unpack(request.data(), request.size());
        msgpack::object obj = oh.get();
        obj.convert(*rq);
        return rq;
    }

    template<typename T>
    std::string Serialize(const T& rp) {
        std::stringstream stream;
        msgpack::pack(stream, rp);
        return stream.str();
    }
};

#endif //HIGHLOAD_SERVER_PARSER_H
