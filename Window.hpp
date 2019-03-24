#pragma once

#include <iostream>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <locale.h>
#include <pthread.h>


class Window {
public:
    WINDOW *header;
    WINDOW *output;
    WINDOW *online;
    WINDOW *input;
    pthread_mutex_t lock;

public:
    Window()
    {
        setlocale(LC_ALL,"");
        initscr();
        curs_set(0);
        pthread_mutex_init(&lock, NULL);
    }

    void SafeWrefresh(WINDOW *w)
    {
        pthread_mutex_lock(&lock);
        wrefresh(w);
        pthread_mutex_unlock(&lock);
    }
    void DrawHeader()
    {
        int h = LINES * 0.2;
        int w = COLS;
        int y = 0;
        int x = 0;
        header = newwin(h, w, y, x);
        box(header, '|' ,'~');
        SafeWrefresh(header);
    }
    
    void DrawOutput()
    {
        int h = LINES * 0.6;
        int w = COLS * 0.7;
        int y = LINES * 0.2;
        int x = 0;
        output = newwin(h, w, y, x);
        box(output, '|' ,'~');
        SafeWrefresh(output);
    }
    
    void DrawOnline()
    {
        int h = LINES * 0.6;
        int w = COLS * 0.3;
        int y = LINES * 0.2;
        int x = COLS * 0.7;
        online = newwin(h, w, y, x);
        box(online, 0, 0);
        SafeWrefresh(online);
    }
    
    void DrawInput()
    {
        int h = LINES * 0.2;
        int w = COLS;
        int y = LINES * 0.8;
        int x = 0;
        input = newwin(h, w, y, x);
        box(input, 0, 0);
        const std::string tips = "Please Enter# ";
        PutStringToWin(input, 2, 2, tips);
        SafeWrefresh(input);
    }

    void PutStringToWin(WINDOW *w, int y , int x, const std::string &message)
    {  
        mvwaddstr(w, y, x, message.c_str());
        SafeWrefresh(w);
    }
    
    void GetStringFromInput(std::string &message)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        wgetnstr(input, buffer, sizeof(buffer));
        message = buffer;
        delwin(input);
        DrawInput();
    }

    void PutMessageToOutput(const std::string &message)
    {
        static int line = 1;
        int y,x;
        getmaxyx(output, y, x);
        if (line > y-2)
        {
            delwin(output);
            DrawOutput();
            line = 1;
        }
        PutStringToWin(output, line++, 2, message);
    }

    //把用户放入online窗口中
    void PutUserToOnline(std::vector<std::string> &online_user)
    {
        int size = online_user.size();
        for (int i = 0; i < size; i++)
        {
            PutStringToWin(online, i + 1, 2, online_user[i]);
        }
    }

    //让标题往跑
    void Welcome()
    {
        const std::string welcome = "welcome to my chat system";
        int hang = 1;
        int y,x;
        int dir = 0; //left->right else
        for(;;)
        {
            DrawHeader();
            getmaxyx(header, y, x);
            PutStringToWin(header, y/2, hang, welcome);
            if (dir == 0)
            {
                hang++;
            } else {
                hang--;
            }
            if (hang > x-welcome.size()-2)
            {
                dir = 1;
            } 
            if (hang <= 1) 
            {
                dir = 0;
            }
            usleep(100000); 
            delwin(header);
        }
    }

    ~Window()
    {
        endwin();
        pthread_mutex_destroy(&lock);
    }
};
