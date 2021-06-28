#ifndef HIGHLOAD_SERVER_STATUSSETTER_H
#define HIGHLOAD_SERVER_STATUSSETTER_H

#include <mutex>

class StatusSetter {
public:
    StatusSetter(): status("OK"), mu(){}

    bool SetStatus(const std::string& new_status) {
        std::lock_guard<std::mutex> lock(mu);
        if (new_status == status) {
            return false;
        }
        status = new_status;
        return true;
    }

private:
    std::string status;
    std::mutex mu;
};

#endif //HIGHLOAD_SERVER_STATUSSETTER_H
