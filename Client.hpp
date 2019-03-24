#pragma once
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include "pro.hpp"
#include "Message.hpp"
#include "Window.hpp"
#include <locale.h>


#define TCP_PORT 8080
#define UDP_PORT 8888


class ChatClient;
struct ParamPair {
    Window *wp;
    ChatClient *cp;
};

ChatClient* pc = nullptr;

void quit(int sig);
class ChatClient
{
public:
    ChatClient(std::string ip_):peer_ip(ip_)
    {
        id = 0;
        tcp_sock = -1;
        udp_sock = -1;
        server.sin_family = AF_INET;
        server.sin_port = htons(UDP_PORT);
        server.sin_addr.s_addr = inet_addr(peer_ip.c_str());
    }

    void InitClient()
    {
        tcp_sock = SocketApi::Socket(SOCK_STREAM);
        udp_sock = SocketApi::Socket(SOCK_DGRAM);

    }

    bool ConnectServer()
    {
        tcp_sock = SocketApi::Socket(SOCK_STREAM);
        return SocketApi::Connect(tcp_sock, peer_ip, 8080);
    }

    bool Register()
    {
        if (Util::ReigsterEnter(nick_name, school, passwd) && ConnectServer()) 
        {
            Request rq;
            rq.method = "REGISTER\n";
            
            Json::Value root;
            root["name"] = nick_name;
            root["school"] = school;
            root["passwd"] = passwd;
            Util::Seralizer(root, rq.text);

            rq.content_length = "Content-Length: ";
            rq.content_length += Util::IntToString((rq.text).size());
            rq.content_length += '\n';

            Util::SendRequest(tcp_sock, rq);
            recv(tcp_sock, &id, sizeof(id), 0);
            bool tes = false;

            if (id >= 10000) 
            {
                std::cout << "register success! your login id :" << id << std::endl;
                tes = true;
            } else {
                std::cout << "r f code is" << id << std::endl;
            }
            close(tcp_sock);
            return tes;
        }
    }


    bool Login()
    {
        if (Util::LoginEnter(id, passwd) && ConnectServer()) 
        {
            Request rq;
            rq.method = "LOGIN\n";
            
            Json::Value root;
            root["id"] = id;
            root["passwd"] = passwd;
            Util::Seralizer(root, rq.text);

            rq.content_length = "Content-Length: ";
            rq.content_length += Util::IntToString((rq.text).size());
            rq.content_length += "\n";

            Util::SendRequest(tcp_sock, rq);
            unsigned int result = 0;
            recv(tcp_sock, &result, sizeof(result), 0);
            bool tes = false;

            if (result >= 10000) 
            {
                tes = true;
                std::string name_ = "None";
                std::string school_ = "None";
                std::string text_ = "i am login! talk with me";
                unsigned int id_ = result;
                unsigned int type_ = LOGIN_TYPE;
                Message m(name_, school_, text_, id_, type_);
                std::string sendString;
                m.ToSendString(sendString);
                UdpSend(sendString);

                std::cout << " 登录成功 ! " << std::endl;
                tes = true;
            } else {
                std::cout << " 登录失败 ! "<< result << std::endl;
            }
            close(tcp_sock);
            return tes;
        }
    }

    void UdpRecv(std::string &message)
    {
        struct sockaddr_in peer;
        Util::RecvMessage(udp_sock, message, peer);
    }

    void UdpSend(const std::string &message)
    {
        Util::SendMessage(udp_sock, message, server);
    }


    static void *Welcome(void *arg)
    {
        pthread_detach(pthread_self());
        Window *wp = (Window *)arg;
        wp->Welcome();
    }
    
    static void *Input(void *arg)
    {
        pthread_detach(pthread_self());
        struct ParamPair *pptr = (struct ParamPair*)arg;
        Window *wp = pptr->wp;
        ChatClient *cp = pptr->cp;
        
        wp->DrawInput();
        std::string text;
        for(;;) {
            wp->GetStringFromInput(text);
            Message msg(cp->nick_name, cp->school, text, cp->id);
            std::string sendString;
            msg.ToSendString(sendString);
            cp->UdpSend(sendString);
        }
        
    }
    
    void Chat()
    {
        Window w;
        struct ParamPair pp = {&w, this};
        pthread_t h,m;
        pthread_create(&h, NULL, Welcome, &w);
        pthread_create(&m, NULL, Input, &pp);
        
        w.DrawOutput();
        w.DrawOnline();
        std::string recvString;
        std::string showString;
        std::vector<std::string> online;
        for (;;)
        {
            Message msg;
            UdpRecv(recvString);
            msg.ToRecvValue(recvString);
            if(msg.Type() == QUIT_TYPE)
            {
                showString = msg.NickName();
                showString += "-";
                showString += msg.School();
                std::string f = showString;
                Util::deleteUser(online, f);
                showString += "# ";
                showString += msg.Text();
                w.DrawOnline();
            } else {
                if (msg.Type() == LOGIN_TYPE)
                {
                }
                showString = msg.NickName();
                showString += "-";
                showString += msg.School();
                std::string f = showString;
                Util::addUser(online, f);
                showString += "# ";
                showString += msg.Text();
            }
                
            w.PutMessageToOutput(showString);
            w.PutUserToOnline(online);   
        }    
    }

    
    ~ChatClient()
    {}

private:
    std::string peer_ip;
    struct sockaddr_in server;
    std::string passwd;
public:
    int tcp_sock;
    int udp_sock;
    unsigned int id;
    std::string nick_name;
    std::string school;
};


void quit(int sig)
{
    std::string name_ = "None";
    std::string school_ = "None";
    std::string text_ = "See you later, wo quit lalala !!!!";
    unsigned int id_ = pc->id;
    unsigned int type_ = QUIT_TYPE;
    Message m(name_, school_, text_, id_, type_);
    std::string sendString;
    m.ToSendString(sendString);
    pc->UdpSend(sendString);
    std::cout << "成功退出" << std::endl;
    endwin();
    close(pc->tcp_sock);
    _exit(0);
    
}

