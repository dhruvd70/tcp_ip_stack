/*
* @Name :       network.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#ifndef __NETWORK__
#define __NETWORK__

#include "../common.h"
#include "../graph/graph.h"
// #include "graph/net_params.h"

#define IF_MAC(pIntf)           ((pIntf)->intf_nw_cfg.mac_addr.mac_addr)
#define IF_IP(pIntf)            ((pIntf)->intf_nw_cfg.ip_addr.ip_addr)
#define NODE_LB_ADDR(pNode)     ((pNode)->node_nw_cfg.lb_ip_addr.ip_addr)

bool node_set_loopback_addr(node_t *pNode, char *lb_addr);

bool node_set_intf_ip_addr(node_t *pNode, char *local_intf, char *ip_addr, char mask);

bool node_unset_intf_ip_addr(node_t *pNode, char *local_intf);

void dump_intf_nw_prop(interface_t *intf);

void dump_node_nw_prop(node_t *pNode);

void dump_nw_graph(graph_t *graph);

unsigned int convert_ip_from_str_to_int(char *ip_addr);

void convert_ip_from_int_to_str(unsigned int ip_addr, char *output_buffer);

char *pkt_buffer_shift_right(char *pkt, unsigned int pkt_size, unsigned int total_buffer_size);

#endif