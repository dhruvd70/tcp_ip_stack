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
    start_shell();

    // dump_graph(topo);
    // dump_nw_graph(topo);

    return 0;
}