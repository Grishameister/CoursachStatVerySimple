#ifndef HIGHLOAD_SERVER_IDBCONNECTION_H
#define HIGHLOAD_SERVER_IDBCONNECTION_H

#include <sw/redis++/redis++.h>
#include <iostream>

class IDBConnection {
public:
    virtual Counters ReadData(const std::string &key) = 0;

    virtual bool WriteData(const std::string &key, const Counters &counters) = 0;
};

class DBAccess : public IDBConnection {
public:
    DBAccess(const std::string& connection) : redis(sw::redis::Redis(connection)) {
    }

    ~DBAccess() = default;

    Counters ReadData(const std::string& key) override {
        std::unordered_map<std::string, std::string> map_counters;
        redis.hgetall(key, std::inserter(map_counters, map_counters.begin()));

        Counters result{};

        try {
            auto it = map_counters.find(PEOPLE);
            if (it == map_counters.end()) {
                return {};
            }
            result.people = std::stoi(it->second);

            it = map_counters.find(BULDOZERS);
            if (it == map_counters.end()) {
                return {};
            }
            result.buldozers = std::stoi(it->second);

            it = map_counters.find(EXCAVATORS);
            if (it == map_counters.end()) {
                return {};
            }
            result.excavators = std::stoi(it->second);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            return result;
        }

        return result;
    }

    bool WriteData(const std::string &key, const Counters& counters) override {
        std::map<std::string, size_t> map_counters;
        map_counters.emplace(PEOPLE, counters.people);
        map_counters.emplace(BULDOZERS, counters.buldozers);
        map_counters.emplace(EXCAVATORS, counters.excavators);

        std::cout << counters << std::endl;
        return redis.hset(key, map_counters.begin(), map_counters.end());
    }

private:
        sw::redis::Redis redis;
};

#endif //HIGHLOAD_SERVER_IDBCONNECTION_H
