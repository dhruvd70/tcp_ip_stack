/*
* @Name :       test_app.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "graph/graph.h"
#include "topologies/topologies.h"

int main()
{
    graph_t* topo = build_generic_topo();

    dump_graph(topo);

    return 0;
}