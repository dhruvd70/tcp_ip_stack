/*
* @Name :       network.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "network.h"
#include <time.h>

void assign_mac_to_intf(interface_t *intf) 
{
    static int counter = 0;
    char temp[15];
    srand(time(NULL) + counter);  // Initialize random seed with time and counter
    sprintf(temp, "%X%X%X%X%X%X",rand() % 256, rand() % 256, rand() % 256,
                                 rand() % 256, rand() % 256, rand() % 256);
    memcpy(IF_MAC(intf), &temp, MAC_ADDR_SIZE);

    counter++;
}

bool node_set_loopback_addr(node_t *pNode, char *lb_addr)
{
    assert(lb_addr);

    pNode->node_nw_cfg.b_lb_addr_cfg = true;
    strncpy(NODE_LB_ADDR(pNode), lb_addr, IP_ADDR_SIZE);
    NODE_LB_ADDR(pNode)[IP_ADDR_SIZE - 1] = '\0';

    return true;
}

bool node_set_intf_ip_addr(node_t *pNode, char *local_intf, char *ip_addr, char mask)
{
    interface_t *intf = get_node_intf_by_name(pNode, local_intf);

    intf->intf_nw_cfg.b_ip_addr_cfg = true;
    strncpy(IF_IP(intf), ip_addr, IP_ADDR_SIZE);
    IF_IP(intf)[IP_ADDR_SIZE - 1] = '\0';
    intf->intf_nw_cfg.mask_val = mask;

    return true;
}

bool node_unset_intf_ip_addr(node_t *pNode, char *local_intf)
{
    return false;
}

void dump_intf_nw_prop(interface_t *intf)
{
    dump_interface(intf);
    if(intf->intf_nw_cfg.b_ip_addr_cfg) {
        printf("\tIP ADDR = %s/%u",IF_IP(intf), intf->intf_nw_cfg.mask_val);
    }
    else {
        printf("\tIP ADDR = %s/%u","NIL", 0);
    }
    printf("\t MAC = %X:%X:%X:%X:%X:%X\n",  IF_MAC(intf)[0], IF_MAC(intf)[1],
                                            IF_MAC(intf)[2], IF_MAC(intf)[3],
                                            IF_MAC(intf)[4], IF_MAC(intf)[5]);
}

void dump_node_nw_prop(node_t *pNode)
{
    interface_t *intf;
    printf("NODE NAME = %s :\n",pNode->node_name);
    for (int i = 0; i < MAX_INTF_PER_NODE; i++) {
        intf = pNode->intf[i];
        if(!intf) {
            break;
        }
    }
    printf("\n");
}

void dump_nw_graph(graph_t *graph)
{
    node_t *pNode;
    glthread_t *curr;
    interface_t *intf;

    printf("\n\t\tTOPOLOGY NAME = %s\n\n", graph->topo_name);

    ITERATE_GLTHREAD_BEGIN(&graph->node_list, curr) {
        pNode = graph_glue_to_node(curr);
        dump_node_nw_prop(pNode);
        for( int i = 0; i < MAX_INTF_PER_NODE; i++){
            intf = pNode->intf[i];
            if(!intf) break;
            dump_intf_nw_prop(intf);
            printf("\n");
        }
        printf("\n");
    }
    ITERATE_GLTHREAD_END(&graph->node_list, curr);
}