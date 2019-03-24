#pragma once
#include <pthread.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <arpa/inet.h>

//用户信息
class User {
private:
    std::string nick_name;
    std::string school;
    std::string passwd;

public:
    User()
    {}
    User(const std::string& nick_name_, const std::string& school_, 
         const std::string& passwd_)
        :nick_name(nick_name_),
        school(school_),
        passwd(passwd_)
    {}

    bool IsPasswdOk(const std::string &passwd_)
    {
        return passwd == passwd_ ? true : false;
    }

    std::string &GetNickName()
    {
        return nick_name;
    }

    std::string &GetSchool()
    {
        return school;
    }

    ~User()
    {}
};


class UserManager {
private:
    unsigned int assgin_id;    //用户注册成功后给用户派发的id
    std::unordered_map<unsigned int, User> users;
    std::unordered_map<unsigned int, struct sockaddr_in> online_users; 
    //sock的在不同电脑可能改变所以直接传入sock的结构体
    pthread_mutex_t lock;
    void Lock()
    {
        pthread_mutex_lock(&lock);
    }
    void Unlock()
    {
        pthread_mutex_unlock(&lock);
    }

public:
    UserManager():assgin_id(10000)
    {
        pthread_mutex_init(&lock, NULL);
    }

    unsigned int Insert(const std::string &n_, 
                        const std::string &s_,
                        const std::string &p_)
    {
        Lock();
        unsigned int id = assgin_id++;//这里分配id
        User u(n_, s_, p_);
        if (users.find(id) == users.end())
        {
            //users.insert(make_pair(id, u));
            users.insert({id, u});
            Unlock();
            return id;
        }
        Unlock();
        return 1;
    }

    unsigned int Check(unsigned int &id, const std::string &passwd)
    {
        Lock();
        auto it = users.find(id);
        if (it != users.end())
        {
            User &u = it->second;
            if (u.IsPasswdOk(passwd))
            {
                Unlock();
                return id;
            }
        }
        Unlock();
        return 2;
    }

    void AddOnlineUser(unsigned int id, struct sockaddr_in &peer)
    {
        Lock();
        auto it = online_users.find(id);
        if (it == online_users.end())
        {
            online_users.insert({id, peer});
        }
        Unlock();
    }

    unsigned int deleteOnlineUser(unsigned int id)
    {
        Lock();
        auto it = online_users.find(id);
        if (it == online_users.end())
        {
            online_users.erase(it);
            return id;
        }
        Unlock();
        return 0;
    }

    void GetUserInfo(const unsigned int &id, std::string &name_,
                     std::string &school_)
    {
        Lock();
        name_ = users[id].GetNickName();
        school_ = users[id].GetSchool();
        Unlock();
    }
    std::unordered_map<unsigned int, struct sockaddr_in> OnlineUser()
    {
        Lock();
        auto online = online_users;
        Unlock();
        return online;
    }
    ~UserManager()
    {
        pthread_mutex_destroy(&lock);
    }
};
