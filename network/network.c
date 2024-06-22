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

    return TRUE;
}

bool node_set_intf_ip_addr(node_t *pNode, char *local_intf, char *ip_addr, char mask)
{
    interface_t *intf = get_node_intf_by_name(pNode, local_intf);
    assert(intf);
    
    intf->intf_nw_cfg.b_ip_addr_cfg = true;
    strncpy(IF_IP(intf), ip_addr, IP_ADDR_SIZE);
    IF_IP(intf)[IP_ADDR_SIZE - 1] = '\0';
    intf->intf_nw_cfg.mask_val = mask;

    return TRUE;
}

bool node_unset_intf_ip_addr(node_t *pNode, char *local_intf)
{
    return FALSE;
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

char *pkt_buffer_shift_right(char *pkt, unsigned int pkt_size, unsigned int total_buffer_size)
{
    char *temp = NULL;
    bool_e temp_mem_req = FALSE;

    if(pkt_size * 2 > total_buffer_size) {
        temp_mem_req = TRUE;
    }

    if(temp_mem_req) {
        temp = calloc(1, pkt_size);
        memcpy(temp, pkt, pkt_size);
        memset(pkt, 0, total_buffer_size);
        memcpy(pkt + (total_buffer_size - pkt_size), temp, pkt_size);
        free(temp);

        return (pkt +(total_buffer_size - pkt_size));
    }
    memcpy(pkt + (total_buffer_size - pkt_size), pkt, pkt_size);
    memset(pkt, 0, pkt_size);

    return (pkt +(total_buffer_size - pkt_size));
}

interface_t *get_matching_subnet_intf(node_t *node, char *ip_addr)
{
    interface_t *intf;
    char *intf_addr = NULL;
    char mask;
    char intf_subnet[16];
    char subnet[16];

    for(int i=0; i<MAX_INTF_PER_NODE; i++) {
        intf = node->intf[i];
        
        if(!intf) {
            return NULL;
        }
        if(intf->intf_nw_cfg.b_ip_addr_cfg == FALSE) {
            continue;
        }

        intf_addr = IF_IP(intf);
        mask = intf->intf_nw_cfg.mask_val;

        memset(intf_subnet, 0, 16);
        memset(subnet, 0, 16);
        
        apply_mask(intf_addr, mask, intf_subnet);
        apply_mask(ip_addr, mask, subnet);

        if(strncmp(intf_subnet, subnet, 16) == 0) {
            return intf;
        }
    }
    return NULL;
}