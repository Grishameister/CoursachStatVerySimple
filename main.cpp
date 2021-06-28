#include <iostream>
#include "Server.h"
#include "ConfigHelper.h"

int main() {
    try {
        ConfigHelper c("../httpd.conf");
        Server s(c.GetLimit(), c.GetHost(), c.GetPort(), c.GetConnection());
        s.run();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
