cc=g++
server=Server
client=Client
INCLUDE=-I /home/mmc/workpress/jsoncpp
LIB_PATH=/home/mmc/workpress/qq/lib/libjsoncpp.a
LDFLAGS=-std=c++11 -lpthread  

.PHONY:all
all:$(server) $(client)

$(server):Server.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS) -static

$(client):Client.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH)  $(LDFLAGS) -lncurses -lncursesw 

.PHONY:clean
clean:
	rm -f $(server) $(client)
