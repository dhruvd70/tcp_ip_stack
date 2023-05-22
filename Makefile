CC=gcc
CFLAGS=-g
TARGET:test.out

OBJS=gluethread/glthread.o \
		graph/graph.o	   \
		topologies/topologies.o

test.out:test_app.o ${OBJS}
	${CC} ${CFLAGS} test_app.o ${OBJS} -o test.out

testapp.o:testapp.c
	${CC} ${CFLAGS} -c test_app.c -o test_app.o

gluethread/glthread.o:gluethread/glthread.c
	${CC} ${CFLAGS} -c -I gluethread gluethread/glthread.c -o gluethread/glthread.o
graph.o:graph/graph.c
	${CC} ${CFLAGS} -c -I . graph/graph.c -o graph/graph.o
topologies.o:topologies/topologies.c
	${CC} ${CFLAGS} -c -I . topologies/topologies.c -o topologies/topologies.o

clean:
	rm **/*.o
	rm test.out test_app.o
