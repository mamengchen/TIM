#include <iostream>
#include <signal.h>
#include "Client.hpp"
#include "Message.hpp"

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << "ip" << std::endl;
}

static void Menu(int &s)
{
    std::cout << "*********************************************" << std::endl;
    std::cout << "*******  1.Register     2.Login  ************" << std::endl;
    std::cout << "***********      3.exit     *****************" << std::endl;
    std::cout << "*********************************************" << std::endl;
    std::cout << "*********************************************" << std::endl;
    std::cout << "*********************************************" << std::endl;
    std::cout << "please select:> ";
    std::cin >> s;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    signal(SIGINT, quit);
    ChatClient cp(argv[1]);
    pc = &cp;
    cp.InitClient();
    int s;
    while(1) {
        Menu(s);
        switch(s) {
        case 1:
            //注册
            cp.Register();
            break;
        case 2:
            //登录
            if (cp.Login())
            {
                cp.Chat();
              //  cp.Logout();
            }
            break;
        case 3:
            exit(0);
            break;
        default:
            exit(1);
            break;
        }
    }

    return 0;
}

