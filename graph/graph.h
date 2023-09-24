/*
* @Name :       graph.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#ifndef __GRAPH__
#define __GRAPH__

#include "../common.h"
#include "../gluethread/glthread.h"
#include "net_params.h"

#include <assert.h>

#define INTF_NAME_SIZE          16
#define NODE_NAME_SIZE          16
#define TOPO_NAME_SIZE          32

#define MAX_INTF_PER_NODE       10

typedef struct link_ link_t;
typedef struct node_ node_t;

typedef struct interface_
{
    char intf_name[INTF_NAME_SIZE];
    struct node_ *att_node;
    struct link_ *link;
    intf_nw_prop_t intf_nw_cfg;
}interface_t;

struct link_
{
    interface_t intf_1;
    interface_t intf_2;
    unsigned int cost;
};

struct node_
{
    char node_name[NODE_NAME_SIZE];
    interface_t *intf[MAX_INTF_PER_NODE];
    node_nw_prop_t node_nw_cfg;
    unsigned int udp_port_number;
    int udp_socket_fd; 
    glthread_t graph_glue;
};

typedef struct graph_
{
    char topo_name[TOPO_NAME_SIZE];
    glthread_t node_list;
}graph_t;

void assign_mac_to_intf(interface_t *intf); //Forward declaring func to prevent recursive calling of .h

GLTHREAD_TO_STRUCT(graph_glue_to_node, node_t, graph_glue);

node_t* graph_add_node(graph_t* graph, char* node_name);

graph_t* graph_create_new(char* topo_name);

void graph_insert_link(node_t* pNode_1, node_t* pNode_2, char* intf_from, char* intf_to, unsigned int cost);

void dump_graph(graph_t *graph);

void dump_node(node_t *node);

void dump_interface(interface_t *interface);

static inline node_t* get_nbr_node(interface_t *interface)
{
    assert(interface->att_node);
    assert(interface->link);

    link_t* link = interface->link;
    if(&link->intf_1 == interface) {
        return link->intf_2.att_node;
    }
    else {
        return link->intf_1.att_node;
    }
}

static inline int get_node_intf_avail_slot(node_t *pNode)
{
    for (int i = 0; i < MAX_INTF_PER_NODE; i++) {
        if(pNode->intf[i]) {
            continue;
        }
        return i;
    }
    return -1;
}

static inline interface_t* get_node_intf_by_name(node_t* pNode, char* intf_name)
{
    interface_t* intf;

    for (int i = 0; i < MAX_INTF_PER_NODE; i++) {
        intf = pNode->intf[i];
        if(!intf) {
            return NULL;
        }
        if(strncmp(intf->intf_name, intf_name, INTF_NAME_SIZE) == 0) {
            return intf;
        }
    }
    return NULL;
}

static node_t* get_node_by_node_name(graph_t* topo, char* node_name)
{
    node_t *node;
    glthread_t *curr;    

    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr) {
        node = graph_glue_to_node(curr);
        if(strncmp(node->node_name, node_name, strlen(node_name)) == 0) {
            return node;
        } 
    } 
    ITERATE_GLTHREAD_END(&topo->node_list, curr);
    return NULL;
}

#endif 