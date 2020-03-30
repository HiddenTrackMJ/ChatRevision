#include "SocketClient.h"

#include <WS2tcpip.h>
#include <winsock2.h>

#include <cassert>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::endl;
using std::string;
using std::thread;

void SocketClient::start() {
    if (OK == connectServer()) {
        startSelecting();
    }
    else  cout << "连接至服务器： [" << SERVER_IP << ":" << SERVER_PORT << "]失败 .";
}

int SocketClient::connectServer() {

    SOCKADDR_IN targetAddr = { 0 };
    targetAddr.sin_family = AF_INET;
    // targetAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, SERVER_IP.data(), (void*)&targetAddr.sin_addr.s_addr);
    targetAddr.sin_port = htons(SERVER_PORT);

    WSADATA wsaData = {};

    if (WSAStartup(SOCKET_VERSION, &wsaData) != OK) {
        cout << "WSAStartup error." << endl;
        return ERR;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect(clientSocket, (SOCKADDR*)&targetAddr, sizeof(SOCKADDR)) ==
        SOCKET_ERROR) {
        cout << "socket connet failed." << endl;
        closesocket(clientSocket);
        WSACleanup();
        return ERR;
    }

    cout << "已连接至服务器： [" << SERVER_IP << ":" << SERVER_PORT << "] ."
        << endl;

    return OK;
}

int SocketClient::socketSend(const string& msg) {
    const char* data = msg.data();

    if (send(clientSocket, data, msg.size(), 0) == SOCKET_ERROR) {
        cout << "send msg err: data=" << data << endl;
        closesocket(clientSocket);
        WSACleanup();
        return ERR;
    }

    // cout << "send msg end:" << sendBuff << endl;

    return OK;
}

std::tuple<int, string> SocketClient::socketReceive() {
    char buff[BUFFER_SIZE]{};
    int rcvLen = recv(clientSocket, buff, BUFFER_SIZE - 1, 0);
    if (rcvLen < 0) {
        cout << "socket [" << static_cast<int>(clientSocket) << "] disconnected."
            << endl;
        return { rcvLen, "" };
    }
    else {
        // cout << "debug: receive data len=[" << rcvLen << "]" << endl;
        string data{ buff };
        return { rcvLen, data };
    }
}
void SocketClient::inputHandlerFunc() {
    while (true) {
        char sendBuf[100];
        std::cin >> sendBuf;
        socketSend(sendBuf);
    }
}

int SocketClient::startSelecting() {
    thread t{ &SocketClient::selecting, std::ref(*this) };
    t.detach();
    isSelecting = true;
    thread handler(&SocketClient::inputHandlerFunc, std::ref(*this));
    handler.join();
    selecting();
    return OK;
}

void SocketClient::selecting() {
    assert(clientSocket != INVALID_SOCKET);

    //cout << "Start select." << endl;

    char buff[BUFFER_SIZE] = {};

    while (true) {
        fd_set fdRead;

        FD_ZERO(&fdRead);

        FD_SET(clientSocket, &fdRead);

        timeval timeInterval = { 1, 0 };

        int ret =
            select(clientSocket + 1, &fdRead, nullptr, nullptr, &timeInterval);
        if (ret < 0) {
            cout << "select task complete." << endl;
            break;
        }
        
        if (FD_ISSET(clientSocket, &fdRead)) {
            FD_CLR(clientSocket, &fdRead);

            int len;
            string eventStr;
            std::tie(len, eventStr) = socketReceive();
            if (len < 0) {
                cout << "ERROR: socket disconneted." << endl;
                break;
            }
            else if (len == 0) {
                // do nothing.
            }
            else {
                cout << eventStr << endl;
            }
        }
        
    }
    cout << "SocketClient selecting done." << endl;
    isSelecting = false;
  
}
