//
// Created by g on 2019.05.01..
//

#ifndef UNTITLED2_UART_H
#define UNTITLED2_UART_H

#include <cstdint>
typedef uint64_t ulli;
using namespace::std;


class UART {
private:
        const int fd;
        static const char* port;

        int static setup();
        bool static const checkCS(ulli szam);
public:
        UART();
        ~UART();

        ulli static const CS(ulli szam);
	int const getFd() {return fd;}
        void const serialSend(ulli packet);
        ulli const serialRecive();
};



#endif //UNTITLED2_UART_H

