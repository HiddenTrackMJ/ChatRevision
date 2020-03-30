#pragma once
#include <iostream>
#include <string>

#define OK 0
#define ERR -1

#define BUFFER_SIZE 1024
#define SOCKET_VERSION MAKEWORD(2, 2)

const int BACKLOG = 15;
const std::string SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 12345;