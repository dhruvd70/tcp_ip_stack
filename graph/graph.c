/*
* @Name :       graph.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "graph.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>


node_t* graph_add_node(graph_t* graph, char* node_name)
{
    node_t *node = (node_t *)calloc(1, sizeof(node_t));
    strncpy(node->node_name, node_name, NODE_NAME_SIZE);
    node->node_name[NODE_NAME_SIZE - 1] = '\0';

    init_glthread(&node->graph_glue);
    glthread_add_next(&graph->node_list, &node->graph_glue);
    return node;
}

graph_t* graph_create_new(char* topo_name)
{
    graph_t *graph = (graph_t *)calloc(1, sizeof(graph_t));
    strncpy(graph->topo_name, topo_name, TOPO_NAME_SIZE);
    graph->topo_name[TOPO_NAME_SIZE - 1] = '\0';

    init_glthread(&graph->node_list);
    return graph;
}

void graph_insert_link(node_t* pNode_1, node_t* pNode_2, char* intf_from, char* intf_to, unsigned int cost)
{
    int empty_intf_slot;
    link_t *link = (link_t *)calloc(1, sizeof(link_t));

    strncpy(link->intf_1.intf_name, intf_from, INTF_NAME_SIZE);
    link->intf_1.intf_name[INTF_NAME_SIZE - 1] = '\0';
    strncpy(link->intf_2.intf_name, intf_to, INTF_NAME_SIZE);
    link->intf_2.intf_name[INTF_NAME_SIZE - 1] = '\0';

    link->intf_1.link = link;
    link->intf_2.link = link;

    link->intf_1.att_node = pNode_1;
    link->intf_2.att_node = pNode_2;
    link->cost = cost;

    empty_intf_slot = get_node_intf_avail_slot(pNode_1);
    pNode_1->intf[empty_intf_slot] = &link->intf_1;

    empty_intf_slot = get_node_intf_avail_slot(pNode_2);
    pNode_2->intf[empty_intf_slot] = &link->intf_2;

    init_nw_intf_prop(&link->intf_1.intf_nw_cfg);
    init_nw_intf_prop(&link->intf_2.intf_nw_cfg);

    assign_mac_to_intf(&link->intf_1);
    assign_mac_to_intf(&link->intf_2);

}

void dump_graph(graph_t *graph)
{
    node_t *node;
    glthread_t *curr;

    printf("\n\t\tTOPOLOGY NAME = %s\n\n", graph->topo_name);

    ITERATE_GLTHREAD_BEGIN(&graph->node_list, curr) {
        node = graph_glue_to_node(curr);
        dump_node(node);    
    }
    ITERATE_GLTHREAD_END(&graph->node_list, curr);
}

void dump_node(node_t *node)
{
    unsigned int i = 0;
    interface_t *intf;

    printf("NODE NAME = %s : \n", node->node_name);
    for(i = 0 ; i < MAX_INTF_PER_NODE; i++) {
        intf = node->intf[i];
        if(!intf) break;
        dump_interface(intf);
    }
    printf("\n");
}

void dump_interface(interface_t *interface)
{
    link_t *link = interface->link;
    node_t *nbr_node = get_nbr_node(interface);
    printf("\tINTF NAME = %s\t NBR NODE = %s\tCOST = %u\n", interface->intf_name,
                                                           nbr_node->node_name, 
                                                           link->cost);
}