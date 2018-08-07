#pragma once


#include <WS2tcpip.h>
#include <cstdint>



#pragma comment (lib, "ws2_32.lib")


class TcpServer
{
    static constexpr int MAX_COLUMNS = 180;
    WSADATA     wsData;
    WORD        SockVer;
    SOCKADDR_IN wsSockAddrIn;
    SOCKET      wsLSocket;
    SOCKET     soCli;

    uint8_t     line[180];
    int         cc = 0;

public:
    TcpServer();
    
    DWORD Start(WORD aPort);
    DWORD Run();
    int   Prompt();
    int InputLine();



    virtual ~TcpServer();
};

