#ifndef HIGHLOAD_SERVER_NOTIFIER_H
#define HIGHLOAD_SERVER_NOTIFIER_H

#include "ClientSocket.h"
#include "Parser.h"
#include "SafeQueue.h"

std::string MakePostRequest(const std::string& host, uint32_t port, const std::string& body) {
    std::string result = "POST /notification HTTP/1.1\r\n";
    result.append("Host: ").append(host).append(":").append(std::to_string(port)).append("\r\n"
                   "Content-Type: application/msgpack\r\n"
                   "Content-Length: ").append(std::to_string(body.size())).append( "\r\n\r\n").
            append(body);
    return result;
}

class Notifier {
public:
    explicit Notifier(SafeQueue<Status>& q): m_q(q), can_exit(false), p() {}
    void Run() {
        while (!can_exit) {
            Status message;
            std::cout << " instance " << std::endl;
            m_q.WaitPop(message);
            if (message.status.empty()) {
                can_exit = true;
                continue;
            }
            ClientSocket cl("127.0.0.1", 8881);

            std::string body = p.Serialize(message);
            std::string result = MakePostRequest("127.0.0.1", 8881, body);

            cl.WriteAllBytes(result.data(), result.size());

            std::string answer = cl.ReadAllBytes(512);
            std::cout << answer << std::endl;
        }
    }
private:
    SafeQueue<Status>& m_q;
    Parser p;
    bool can_exit;
};

#endif //HIGHLOAD_SERVER_NOTIFIER_H
