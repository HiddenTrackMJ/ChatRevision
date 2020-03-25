#pragma once
#include <condition_variable>
#include <iostream>
#include <list>
#include <mutex>
#include <sstream>
#include "SocketClient.h"
#include "EventProcessor.h"
#include "constant.h"

using std::cin;
using std::condition_variable;
using std::cout;
using std::endl;
using std::list;
using std::mutex;
using std::string;
using std::stringstream;
using std::unique_ptr;

enum class ConnectRsp { ConnectSuccess, ConnectError, AuthFailure };

enum class UserState {WaitingSighUpOrLogin, WaitingUsername, WaitingPasswd, Connecting, Connected };

class UserAgent : public EventProcessor {
  UserAgent(const UserAgent&) = delete;
  UserAgent(const UserAgent&&) = delete;
  UserAgent operator=(const UserAgent&) = delete;

  UserState currState{UserState::WaitingSighUpOrLogin};
  unique_ptr<SocketClient> socketClient{};
  string currentUser{""};

  void inputHandlerFunc();

  int setupConnection(string serverIp, int serverPort, string username, string passwd);

  // int sendMsg(const string& msg);

  int readSocketData(const SOCKET s, char* const buff, const int buffSize);

  ConnectRsp connectServer(string ip, int port, shared_ptr<LoginEvent> loginMsg);

  int signUp(string ip, int port, shared_ptr<SignUpEvent> signUpMsg);

 protected:
  void processEvent(shared_ptr<Event> evn) override final;

 public:
  UserAgent(){};
  void start();
};
