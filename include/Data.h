#ifndef HIGHLOAD_SERVER_DATA_H
#define HIGHLOAD_SERVER_DATA_H

#include <msgpack.hpp>

#include <string>
#include <string_view>

static constexpr char PEOPLE[] = "people";
static constexpr char BULDOZERS[] = "buildozers";
static constexpr char EXCAVATORS[] = "excavators";

enum class Route {
    WriteStateData = 1,
    ReadStatData,
};

MSGPACK_ADD_ENUM(Route)

enum class Error {
    Success = 1,
    NotFound,
    KeyExist,
};

MSGPACK_ADD_ENUM(Error)

struct Counters {
    long long int people;
    long long int excavators;
    long long int buldozers;

    MSGPACK_DEFINE_MAP(people, excavators, buldozers)

    friend std::ostream &operator<<(std::ostream &os, const Counters& counters);

    Counters(): people(0), excavators(0), buldozers(0){};

    bool HasEmptyFields() const {
        return people == 0 && excavators == 0 && buldozers == 0;
    };
};

inline std::ostream &operator<<(std::ostream &os, const Counters& counters) {
    os << "people: " << counters.people << std::endl
               << "excavators: " << counters.excavators << std::endl
               << "buldozers: " << counters.buldozers << std::endl;
    return os;
}

struct Status {
    std::string status;
    std::string date;

    MSGPACK_DEFINE_MAP(status, date)
};

struct DataRequest {
    Route type;
    Counters counters;
    std::string date;

    MSGPACK_DEFINE_MAP(type, counters, date)

    friend std::ostream &operator<<(std::ostream &os, const DataRequest &rq);
};

inline std::ostream &operator<<(std::ostream &os, const DataRequest &rq) {
    os << "date: " << rq.date << std::endl << rq.counters;
    return os;
}

struct DataResponse {
    Route type;
    Error error;
    Counters counters;
    std::string date;

    MSGPACK_DEFINE_MAP(type, error, counters, date)

    friend std::ostream &operator<<(std::ostream &os, const DataResponse &rp);
};

inline std::ostream &operator<<(std::ostream &os, const DataResponse &rp) {
    os << "date: " << rp.date << std::endl << rp.counters;
    return os;
}

#endif //HIGHLOAD_SERVER_DATA_H
