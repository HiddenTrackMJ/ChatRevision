#include "SocketServer.h"
#include "UserAgent.h"
#include "constant.h"

int main(int argc, char* argv[]) {
  using std::cout;
  using std::endl;
  using std::stoi;

  auto inputError = [&argc, &argv] {
    cout << "input error argc=" << argc << endl;
    if (argc < 2) {
      cout << "input error:" << endl;
      cout << "if you want to start server: server SERVER_PORT." << endl;
      cout << "if you want to start client: client SERVER_HOST SERVER_PORT." << endl;
    } else {
      cout << "error cmd=[" << argv[1] << "]. ";
      cout << "Only [client] or [server] support." << endl;
    }
    return ERR;
  };

  cout << "welcome to ChatServer." << endl;

  if (argc > 1) {
    if (strcmp(argv[1], "client") == 0) {
      if (argc == 4) {
        char* host = argv[2];
        int port = stoi(argv[3], 0, 10);
        cout << "setup socket to [" << host << ":" << port << "]" << endl;
        UserAgent agent{};
        agent.start();
        return OK;
      } else {
        cout << "input error. " << endl;
        cout << "you should input like this: client SERVER_HOST SERVER_PORT" << endl;
        return ERR;
      }
    } else if (strcmp(argv[1], "server") == 0) {
      if (argc == 3) {
        int port = stoi(argv[2], 0, 10);
        cout << "start server on port [" << port << "]" << endl;
        SocketServer server(port);
        server.start();
        return OK;
      } else {
        cout << "input error. " << endl;
        cout << "you should input like this: server SERVER_PORT" << endl;
        return ERR;
      }
    } else {
      return inputError();
    }
  } else {
    return inputError();
  }
}