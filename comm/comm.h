/*
* @Name :       comm.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>
#include <netinet/in.h>
#include <memory.h>
#include <errno.h>
#include <netdb.h> 

#include "../common.h"
#include "../graph/graph.h"

#define MAX_PKT_BUFF_SIZE       2048


void network_start_pkt_rx_thread(graph_t* topo);

int send_pkt_out(char* pkt, unsigned int pkt_size, interface_t* intf);

int pkt_rx(node_t* rx_node, interface_t* intf, char* pkt,
                    unsigned int pkt_size);
