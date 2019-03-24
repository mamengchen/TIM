#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <semaphore.h>

class DataPool
{
private:
    std::vector<std::string> pool;
    int cap;
    sem_t data_sem;//用来保护数据
    sem_t blank_sem;//用来保护格子
    //信号量是用来描述临界资源数量，是一个计数器，但也不但是个计数器
    int product_step;
    int consume_step;

public:
    DataPool(int cap_ = 512):cap(cap_), pool(cap_)
    {
        sem_init(&data_sem, 0, 0);
        sem_init(&blank_sem, 0, cap);
        product_step=0;
        consume_step=0;
    }
    //这里可以加线程池
    void PutMessage(const std::string msg)
    {
        sem_wait(&blank_sem); //p操作，申请空格子资源
        pool[product_step] = msg;
        product_step++;
        product_step %= cap;
        sem_post(&data_sem); //v操作，把数据位置露出来
    }

    void GetMessage(std::string &msg)
    {
        sem_wait(&data_sem);//
        msg = pool[consume_step];
        consume_step++;
        consume_step %= cap;
        sem_post(&blank_sem);
    }

    ~DataPool()
    {
        sem_destroy(&data_sem);
        sem_destroy(&blank_sem);
    }

};
