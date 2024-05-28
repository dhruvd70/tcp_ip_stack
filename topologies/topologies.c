/*
* @Name :       topologies.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "topologies.h"

graph_t* build_generic_topo(void)
{
/*
                           +----------+
                      0/4 |          |0/0
         +----------------+   R0_re  +---------------------------+
         |                |          |                           |
         |                +----------+                           |
         |                                                       |
         |                                                       |
         |                                                       |
         |                                                       |
         |0/5                                                    |0/1
     +---+---+                                              +----+-----+
     |       |0/3                                        0/2|          |
     | R2_re +----------------------------------------------+    R1_re |
     |       |                                              |          |
     +-------+                                              +----------+
*/

    graph_t *topo = graph_create_new("GENERIC GRAPH");
    node_t *R0 = graph_add_node(topo, "R0_re");
    node_t *R1 = graph_add_node(topo, "R1_re");
    node_t *R2 = graph_add_node(topo, "R2_re");

    graph_insert_link(R0, R1, "eth0/0", "eth0/1",1);
    graph_insert_link(R1, R2, "eth0/2", "eth0/3",1);
    graph_insert_link(R0, R2, "eth0/4", "eth0/5",1);
// printf("--1\n");
    node_set_loopback_addr(R0, "127.0.0.1");
// printf("--2\n");
    node_set_intf_ip_addr(R0,"eth0/4", "30.1.1.31", 24);
// printf("--3\n");
    node_set_intf_ip_addr(R0,"eth0/0", "10.1.1.10", 24);
// printf("--4\n");
    node_set_loopback_addr(R1, "127.0.0.2");
    node_set_intf_ip_addr(R1,"eth0/1", "10.1.1.11", 24);
    node_set_intf_ip_addr(R1,"eth0/2", "20.1.1.20", 24);

    node_set_loopback_addr(R2, "127.0.0.3");
    node_set_intf_ip_addr(R2,"eth0/3", "20.1.1.21", 24);
    node_set_intf_ip_addr(R2,"eth0/5", "30.1.1.30", 24);

    network_start_pkt_rx_thread(topo);
    return topo;
}

graph_t* build_linear_topo(void)
{
    graph_t *topo = graph_create_new("LINEAR GRAPH");
    node_t *H1 = graph_add_node(topo, "H1");
    node_t *H2 = graph_add_node(topo, "H2");
    node_t *H3 = graph_add_node(topo, "H3");

    graph_insert_link(H1, H2, "eth0/1", "eth0/2",1);
    graph_insert_link(H2, H3, "eth0/3", "eth0/4",1);

    node_set_loopback_addr(H1, "122.1.1.1");
    node_set_intf_ip_addr(H1,"eth0/1", "10.1.1.1", 24);

    node_set_loopback_addr(H2, "122.1.1.2");
    node_set_intf_ip_addr(H2,"eth0/2", "10.1.1.2", 24);
    node_set_intf_ip_addr(H2,"eth0/3", "20.1.1.2", 24);

    node_set_loopback_addr(H3, "122.1.1.3");
    node_set_intf_ip_addr(H3,"eth0/4", "20.1.1.1", 24);

    network_start_pkt_rx_thread(topo);
    return topo;    
}