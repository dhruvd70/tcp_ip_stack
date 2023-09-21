CC=gcc
CFLAGS=-g
BUILD_DIR=build/
BUILD_DIRS=dir
BIN_DIR=bin/
LIBS=-lpthread -L ./CommandParser -lcli

OBJS=   $(BUILD_DIR)nwcli.o			\
		$(BUILD_DIR)glthread.o 		\
		$(BUILD_DIR)graph.o	   		\
		$(BUILD_DIR)topologies.o	\
		$(BUILD_DIR)network.o       \
		$(BUILD_DIR)test_app.o	 

all: $(BUILD_DIRS) tcp_ip_app 
	 (cd CommandParser; make)

tcp_ip_app:$(BUILD_DIR)test_app.o ${OBJS} CommandParser/libcli.a
	${CC} ${CFLAGS} ${OBJS} -o $(BIN_DIR)tcp_ip_app ${LIBS}
	@echo
	@echo BUILD COMPLETE!!
	@echo execute ./bin/tcp_ip_app to start the app

$(BUILD_DIR)test_app.o:test_app.c
	${CC} ${CFLAGS} -c -I . test_app.c -o $(BUILD_DIR)test_app.o

$(BUILD_DIR)glthread.o:gluethread/glthread.c
	${CC} ${CFLAGS} -c -I . gluethread/glthread.c -o $(BUILD_DIR)glthread.o

$(BUILD_DIR)graph.o:graph/graph.c
	${CC} ${CFLAGS} -c -I . graph/graph.c -o $(BUILD_DIR)graph.o

$(BUILD_DIR)topologies.o:topologies/topologies.c
	${CC} ${CFLAGS} -c -I . topologies/topologies.c -o $(BUILD_DIR)topologies.o

$(BUILD_DIR)network.o:network/network.c
	${CC} ${CFLAGS} -c -I . network/network.c -o $(BUILD_DIR)network.o

$(BUILD_DIR)nwcli.o:nwcli.c
	${CC} ${CFLAGS} -c -I . nwcli.c -o $(BUILD_DIR)nwcli.o

$(BUILD_DIRS):
	@mkdir $(BUILD_DIR)
	@mkdir $(BIN_DIR)

CommandParser/libcli.a:
	(cd CommandParser; make)

.PHONY: clean
clean:
	rm -rf build bin
	(cd CommandParser; make clean)
