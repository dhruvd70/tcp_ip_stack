/*
* @Name :       test_app.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "graph/graph.h"
#include "topologies/topologies.h"
#include "network/network.h"

int main()
{
    graph_t* topo = build_generic_topo();

    // dump_graph(topo);
    dump_nw_graph(topo);

    return 0;
}