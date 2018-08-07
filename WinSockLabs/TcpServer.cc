#include "TcpServer.h"
#include <iostream>
#include <string>
#include <stdint.h>


#define ResetLine ZeroMemory(line, MAX_COLUMNS); cc=0;
#define Echo send(soCli, (char*)line+cc, 1,0)

TcpServer::TcpServer()
{
    SockVer = MAKEWORD(2, 2);
    int wsOK = WSAStartup(SockVer, &wsData);
    if (wsOK != 0){
        std::cerr << "Cannot initialize socket![" << GetLastError() << "]\n";
        return; // GetLastError();
    }
    ResetLine
}

DWORD TcpServer::Start(WORD aPort)
{
    if ((wsLSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
        std::cerr << "Can't initialize the listening socket.\n";
        return GetLastError();
    }
    
    // Binding 
    wsSockAddrIn.sin_family = AF_INET;
    wsSockAddrIn.sin_addr.S_un.S_addr = INADDR_ANY;
    wsSockAddrIn.sin_port = htons(aPort);
    int R = bind(wsLSocket, (sockaddr*)&wsSockAddrIn, sizeof(wsSockAddrIn));
    return R;
}

DWORD TcpServer::Run()
{
    listen(wsLSocket, SOMAXCONN);
    sockaddr_in CliAddrInfo;
    int cisz  = sizeof(sockaddr_in);
    soCli = accept(wsLSocket, (sockaddr*)&CliAddrInfo, &cisz);
    std::cout << "Connection established:\n";

    if (soCli == INVALID_SOCKET){
        return GetLastError();
    }

    char host[NI_MAXHOST];    // Client's hostname.
    char service[NI_MAXHOST]; // Serv ice (i.e. port)
    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&CliAddrInfo, sizeof(sockaddr_in), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
        std::cout << "Remote host(" << host << ") on port" << service << '\n';
    }
    else{
        inet_ntop(AF_INET, &CliAddrInfo.sin_addr, host, NI_MAXHOST);
        std::cout <<'\'' <<  host << "' => on port:" << std::hex <<  ntohs(CliAddrInfo.sin_port) << '\n';
    }

    std::cout << "Closing the listener socket:\n";
    closesocket(wsLSocket);
    char buf[4096];
    bool s_on = true;
    do{
        std::cout << "in rec-send loop:\n";
                
            
        switch (int r = InputLine(); r){
            case -3:
                s_on = false;
                break;
            default:
                if (r < 0){
                    std::cout << " Interractive session stopped by error or by remote program.\n";
                    s_on = false;
                }
                else{
                    std::cout << "line:['" << line << "']\n";
                    std::cout << "Sending Prompt.\n";
                }
                break;
        }
        
                    
            
        //int rc = recv(soCli, buf, 4096, 0);
        //if (rc == SOCKET_ERROR){
        //    std::cerr << "Socket Error on Input (recv)\n";
        //    break;
        //}

        //if (!rc){
        //    std::cout << "(disconnected). Stopping.\n";
        //    break;
        //}

        //buf[rc] = 0;
        //std::cout << " recv: " << rc << "bytes:\n['" << buf << "']\n";
        //for (int i = 0; i < rc; i++)
        //    std::printf("|%d", buf[i]);
        //std::cout << '\n';
        //ZeroMemory(buf, 4096);
        //std::sprintf(buf, "OK:>");
        //rc = send(soCli, buf, 4,0);
        //std::cout << "Sent " << rc << "bytes to host\n";

    } while (s_on);

    std::cout << " Done. Closing client socket and Quitting.\n";
    closesocket(soCli);
    std::cout << " Cleaning up our WSA data.\n";
    WSACleanup();
    std::cout << "Bye.\n";
    return 0;
}

int TcpServer::Prompt()
{
    ResetLine
    int y = send(soCli, "OK:>", 4, 0);
    if (y != 4){
        return -1;
    }

    return y;
}

int TcpServer::InputLine()
{
    ResetLine
    Prompt();
    do{
        int rcv = recv(soCli, (char*)line + cc, 1, 0);
        if (rcv == SOCKET_ERROR){
            std::cerr << "Socket Error on Input (recv)\n";
            return (int)GetLastError() * -1; // Force Negative?
        }

        if (!rcv){
            std::cout << "(disconnected). Stopping.\n";
            return -2;
        }

        Echo;
        switch (line[cc]){
            case '\n':
            case '\r':
                // --cc;
                line[cc] = 0;
                return cc;
            
            case 3:
                // ctrl-c: 
                return -3;
            default:
                ++cc;
                break;
        }
    } while (1);

    return 0;
}



TcpServer::~TcpServer()
{
}
