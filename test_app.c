/*
* @Name :       test_app.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "graph/graph.h"
#include "topologies/topologies.h"
#include "network/network.h"
#include "Command_Parser/libcli.h"

#include "common.h"

extern void nw_init_cli();

graph_t* topo = NULL;

int main()
{
    nw_init_cli();
    topo = build_generic_topo();
    // topo = build_linear_topo();
    if (!topo) {
        printf("TOPOLOGY INITIALIZATION FAILED!\n");
        return 0;
    }
    start_shell();
    return 0;
}