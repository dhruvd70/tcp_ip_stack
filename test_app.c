/*
* @Name :       test_app.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "graph/graph.h"
#include "topologies/topologies.h"
#include "network/network.h"
#include "CommandParser/libcli.h"

#include "common.h"

extern void nw_init_cli();

graph_t* topo = NULL;

int main()
{
    nw_init_cli();
    topo = build_generic_topo();

    sleep(3);// delay to initialize rx thread 
    node_t* snode = get_node_by_node_name(topo, "R0");
    if (snode == NULL) {
        printf("ERROR: get_node_by_node_name RETURNED NULL\n");
    }
    
    interface_t* sintf = get_node_intf_by_name(snode, "eth0/0");
    char msg[] = "hello from dhruv";
    printf("RC = %d\n",send_pkt_out(msg, sizeof(msg), sintf));
    
    start_shell();

    // dump_graph(topo);
    // dump_nw_graph(topo);

    return 0;
}