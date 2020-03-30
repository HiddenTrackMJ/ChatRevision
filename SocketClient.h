#pragma once
//#include "EventProcessor.h"
#include <winsock2.h>

#include <tuple>

#include "common.h"

using std::string;

enum class SC_STATE { Disconnect, Connecting, Connected };

class SocketClient {
    SC_STATE state = SC_STATE::Disconnect;
    SOCKET clientSocket{ INVALID_SOCKET };
    bool isSelecting = false;
    void selecting();

    void inputHandlerFunc();

    int setupConnection(string serverIp, int serverPort, string username, string passwd);

    // int sendMsg(const string& msg);

    int readSocketData(const SOCKET s, char* const buff, const int buffSize);



public:

    void start();

    int connectServer();

    int socketSend(const string& msg);

    std::tuple<int, string> socketReceive();

    int startSelecting();

    int close();
};
