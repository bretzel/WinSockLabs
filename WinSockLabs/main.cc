#include "TcpServer.h"


int main()
{
    TcpServer S;
    S.Start(0xAC6A);
    S.Run();
    return 0;
}