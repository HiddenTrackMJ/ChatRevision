#include "SocketServer.h"

#include <WS2tcpip.h>
#include <winsock2.h>
#include <sstream>

#include <map>
#include <vector>
#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::endl;
using std::string;
using std::vector;

SocketServer::SocketServer(int _port) : port(_port) {}

template<typename T> string toString(const T& t) {
    std::stringstream s;
    s << t;
    return s.str();
}

void SocketServer::selecting() {
    cout << "Start select." << endl;

    char buff[BUFFER_SIZE] = {};
    string sendBuff;

    
    while (true) {
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExcept;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcept);

        FD_SET(serverSocket, &fdRead);
        FD_SET(serverSocket, &fdWrite);
        FD_SET(serverSocket, &fdExcept);

        for (auto gClient : clientsWithUserName) {
            FD_SET(static_cast<SOCKET>(gClient.first), &fdRead);
        }

        // timeval timeInterval = { 1, 0 };

        int ret = select(serverSocket + 1, &fdRead, &fdWrite, &fdExcept, nullptr);
        // cout << "select once..." << endl;

        if (ret < 0) {
            cout << "select task complete." << endl;
            break;
        }

        if (FD_ISSET(serverSocket, &fdRead)) {
            FD_CLR(serverSocket, &fdRead);

            cout << "正在连接..." << endl;
            SOCKADDR_IN clientAddr = {};
            int socketAddrLen = sizeof(SOCKADDR_IN);
            SOCKET clientId = INVALID_SOCKET;
            clientId = accept(serverSocket, (SOCKADDR*)&clientAddr, &socketAddrLen);
            if (INVALID_SOCKET == clientId) {
                cout << "accept client socket error." << endl;
                continue;
            }

            cout << "链接成功: " << clientId << endl;

            clientConnet(clientId);
        }

       

        for (size_t n = 0; n < fdRead.fd_count; n++) {
            auto clientId = fdRead.fd_array[n];

            if (readSocketData(clientId, buff, BUFFER_SIZE) == ERR) {
                cout << "clientId[" << clientId << "] disconnect." << endl;
                kick(clientId);
            }
            else {
                cout << "Receive msg from client socket[" << clientId << "] " << endl;
                string eventStr{ buff };
                for (auto client : clientsWithUserName) {
                    cout << "lalala: " << client.first << endl;
                }

                for (auto client : clientsWithUserName) {
                    sendBuff = "user-" + toString<SOCKET>(client.first) + ": " + eventStr;
                    if(client.first != clientId) sendSocketData(client.first, sendBuff.c_str());
                }
            }
        }
    }

    cout << "End select." << endl;

}

int SocketServer::readSocketData(const SOCKET s, char* const buff, const int buffSize) {
    memset(buff, 0, buffSize);
    int rcvLen = recv(s, buff, buffSize - 1, 0);
    if (rcvLen < 0) {
        cout << "socket [" << static_cast<int>(s) << " disconnected." << endl;
        return ERR;
    }
    else {
        cout << "debug: receive data len=[" << rcvLen << "]" << endl;
        return OK;
    }
}

int SocketServer::sendSocketData(const unsigned int clientId, const string& msg) {
    const char* data = msg.data();

    if (send(clientId, data, msg.size(), 0) == SOCKET_ERROR) {
        cout << "send msg err: data=" << data << endl;
        return ERR;
    }

    cout << "send msg end:" << msg << endl;

    return OK;
}

/*int SocketServer::transferChatMsg(shared_ptr<EventProcessor::ChatMsgEvent> chatMsg) {
    const string& username = chatMsg->toUser;
    auto it = usernameToSocketIdMap.find(username);
    if (it != usernameToSocketIdMap.end()) {
        auto clientId = it->second;
        sendSocketData(clientId, chatMsg->toMsg());
        return OK;
    }
    else {
        cout << "Can not send msg[" << chatMsg->toMsg() << "] to peer[" << chatMsg->toUser << "]"
            << endl;
        return ERR;
    }
}*/

int SocketServer::setupConnect() {
    

    WSADATA wsaData = {};
    if (WSAStartup(SOCKET_VERSION, &wsaData) != OK) {
        cout << "WSAStartup error." << endl;
        return ERR;
    }

    SOCKADDR_IN serverAddr = { 0 };

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        cout << "绑定失败" << endl;
        return ERR;
    }

    if (listen(serverSocket, BACKLOG) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        cout << "监听失败" << endl;
        return ERR;
    }

    cout << "服务器监听: [" << port << "] 成功." << endl;



    return OK;
}

string SocketServer::getUsernameByClientId(unsigned clientId) const {
    auto it = clientsWithUserName.find(clientId);
    if (it != clientsWithUserName.end()) {
        return it->second;
    }
    else {
        return "";
    }
}

bool SocketServer::kick(unsigned int clientId) {
    auto it = clientsWithUserName.find(clientId);
    if (it != clientsWithUserName.end()) {
        auto user = it->second;
        if (user.size() > 0) {
            usernameToSocketIdMap.erase(user);
        }
        cout << "kick clientId[" << clientId << "] username[" << user << "]." << endl;
        clientsWithUserName.erase(clientId);
        closesocket(clientId);
        return true;
    }
    else {
        return false;
    }
}

int SocketServer::start() {
    cout << "正在连接至： [" << port << "] ... " << endl;
    setupConnect();
    selecting();
    return OK;
}

void SocketServer::clientConnet(unsigned int clientId) { clientsWithUserName[clientId] = ""; }

