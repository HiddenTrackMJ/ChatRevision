// TheyChat.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "SocketServer.h"
#include "SocketClient.h"
#include "common.h"

int main(int argc, char* argv[]) {
    using std::cout;
    using std::endl;
    using std::stoi;

    auto inputError = [&argc, &argv] {
        cout << "input error argc=" << argc << endl;
        if (argc < 2) {
            cout << "输入参数错误！！！" << endl;
        }
        else {
            cout << "error cmd=[" << argv[1] << "]. ";
            cout << "输入指令错误！！！" << endl;
        }
        return ERR;
    };

  
    cout << "argc: " << argc << " argv: " << argv << endl;
    if (argc > 1) {
        if (strcmp(argv[1], "client") == 0) {
            if (argc == 2) {
                cout << "*******欢迎来到TheyChat*******" << endl;
                cout << "setup socket to [" << SERVER_IP << ":" << SERVER_PORT << "]" << endl;
                SocketClient client{};
                client.start();
                return OK;
            }
            else {
                cout << "input error. " << endl;
                cout << "you should input like this: client SERVER_HOST SERVER_PORT" << endl;
                return ERR;
            }
        }
        else if (strcmp(argv[1], "server") == 0) {
            if (argc == 2) {
                cout << "start server" << endl;
              
                cout << "start server on port [" << SERVER_PORT << "]" << endl;
                SocketServer server(SERVER_PORT);
                server.start();
                return OK;
            }
            else {
                cout << "input error. " << endl;
                cout << "you should input like this: server SERVER_PORT" << endl;
                return ERR;
            }
        }
        else {
            return inputError();
        }
    }
    else {
        return inputError();
    }
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
