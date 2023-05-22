/*
* @Name :       topologies.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "topologies.h"

graph_t* build_generic_topo()
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
    node_t *R0 = graph_add_node(topo, "R0");
    node_t *R1 = graph_add_node(topo, "R1");
    node_t *R2 = graph_add_node(topo, "R2");

    graph_insert_link(R0, R1, "eth0/0", "eth0/1",1);
    graph_insert_link(R1, R2, "eth0/2", "eth0/3",1);
    graph_insert_link(R0, R2, "eth0/4", "eth0/5",1);

    return topo;
}