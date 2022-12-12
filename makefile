CC=gcc
CFLAGS=-g -Wall
SERVER_TARGET=server
SERVER_OBJS=server.o serverList.o serverSemaphore.o
CLIENT_TARGET=client
CLIENT_OBJS=client.o clientEdit.o clientList.o

MAKEALL: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) -o $(SERVER_TARGET) $(SERVER_OBJS)

server.o: server.c serverList.h serverSemaphore.h serverValues.h
serverList.o: serverList.c serverValues.h
serverSemaphore.o: serverSemaphore.h serverValues.h

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) -o $(CLIENT_TARGET) $(CLIENT_OBJS)

client.o: client.c clientEdit.h clientList.h clientValues.h
clientEdit.o: clientEdit.c clientValues.h
clientList.o: clientList.c clientEdit.h clientValues.h


clean:
	rm -f *.o
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)
