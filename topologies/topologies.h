/*
* @Name :       topologies.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#ifndef __NET_PARAMS__
#define __NET_PARAMS__

#include "../graph/graph.h"
#include "../network/network.h"
#include "../comm/comm.h"

graph_t* build_generic_topo(void);

graph_t* build_linear_topo(void);

#endif