CC=gcc
DB=gdb
CFLAGS=-g
BUILD_DIR=build/
BUILD_DIRS=dir
BIN_DIR=bin/
LIBS=-lpthread -L ./Command_Parser -lcli

OBJS=   $(BUILD_DIR)layer_2.o		\
		$(BUILD_DIR)comm.o			\
		$(BUILD_DIR)nwcli.o			\
		$(BUILD_DIR)glthread.o 		\
		$(BUILD_DIR)graph.o	   		\
		$(BUILD_DIR)topologies.o	\
		$(BUILD_DIR)network.o       \
		$(BUILD_DIR)test_app.o

all: $(BUILD_DIRS) tcp_ip_app 

tcp_ip_app:$(BUILD_DIR)test_app.o ${OBJS} Command_Parser/libcli.a
	${CC} ${CFLAGS} ${OBJS} -o $(BIN_DIR)tcp_ip_app ${LIBS}
	@echo
	@echo BUILD COMPLETE!!

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

$(BUILD_DIR)comm.o:comm/comm.c
	${CC} ${CFLAGS} -c -I . comm/comm.c -o $(BUILD_DIR)comm.o

$(BUILD_DIR)layer_2.o:layer_2/layer_2.c
	${CC} ${CFLAGS} -c -I . layer_2/layer_2.c -o $(BUILD_DIR)layer_2.o

$(BUILD_DIRS):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

Command_Parser/libcli.a:
	(cd Command_Parser; make)

debug: $(BUILD_DIRS) tcp_ip_app
	${DB} ./bin/tcp_ip_app

run: $(BUILD_DIRS) tcp_ip_app
	./bin/tcp_ip_app

.PHONY: clean
clean:
	rm -rf build bin
	rm -f *.txt
	(cd Command_Parser; make clean)
