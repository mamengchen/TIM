#include "Server.hpp"
#include <iostream>

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << "tcp_port udp_port" << std::endl;
}

void *RunProduct(void *arg)
{
    pthread_detach(pthread_self());
    ChatServer *sp = (ChatServer *)arg;
    while(1)
    {
        sp->Product();
    }
    //线程分离
}

void *RunConsume(void *arg)
{
    pthread_detach(pthread_self());
    ChatServer *sp = (ChatServer *)arg;
    while(1)
    {
        sp->Consume();
    }
    //线程分离
}

//我们有两个端口号Server. 有两个端口tcp_port udp_port
int main(int argc, char *argv[])
{
    if (argc != 3) 
    {
        Usage(argv[0]);
        exit(1);
    }

    int tcp_port = atoi(argv[1]);
    //argv是char型的转成整形用atoi
    int udp_port = atoi(argv[2]);

    ChatServer *sp = new ChatServer(tcp_port, udp_port);
    sp->InitServer();
    
    pthread_t c,p;
    pthread_create(&p, NULL, RunProduct, (void *)sp);
    pthread_create(&c, NULL, RunConsume, (void *)sp);
    sp->Start();

    return 0;
}
