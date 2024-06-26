/*
* @Name :       layer_2.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "layer_2.h"
#include "../tcp_const.h"
#include "../comm/comm.h"
#include <arpa/inet.h>
#include <stdlib.h>


void send_arp_broadcast_request(node_t *node, interface_t *out_intf, char *ip_addr)
{
    /*Allocate memory of size ETH_HDR + ARP_HDR*/
    unsigned int payload_size = sizeof(arp_hdr_t);
    ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *)calloc(1, ETH_HDR_SIZE_EXCL_PAYLOAD + payload_size);
    
    if(!out_intf) {
        out_intf = get_matching_subnet_intf(node, ip_addr);
        if(!out_intf) {
            printf(ANSI_COLOR_RED"ERROR: %s : No Eligible subnet for ARP resolution for IP Addr : %s\n"ANSI_COLOR_RESET, node->node_name, ip_addr);
            return;
        }
    }
    /*Preparing ETH_HDR*/
    l2_fill_broadcast_mac(ethernet_hdr->dest_mac_addr.mac_addr);
    memcpy(ethernet_hdr->src_mac_addr.mac_addr, IF_MAC(out_intf), sizeof(mac_addr_t));
    ethernet_hdr->type = ARP_MSG;

    /*Preparing ARP_HDR*/
    arp_hdr_t *arp_hdr = (arp_hdr_t *)ethernet_hdr->payload;

    arp_hdr->hw_type = 1;
    arp_hdr->protocol_type = 0x0800;
    arp_hdr->hw_addr_len = sizeof(mac_addr_t);
    arp_hdr->protocol_addr_len = 4;

    arp_hdr->op_code = ARP_BROADCAST_REQ;
    memcpy(arp_hdr->src_mac_addr.mac_addr, IF_MAC(out_intf), sizeof(mac_addr_t));
    inet_pton(AF_INET, IF_IP(out_intf), &arp_hdr->src_ip_addr);
    arp_hdr->src_ip_addr = htonl(arp_hdr->src_ip_addr);

    memset(arp_hdr->dest_mac_addr.mac_addr, 0, sizeof(mac_addr_t));
    inet_pton(AF_INET, ip_addr, &arp_hdr->dest_ip_addr);
    arp_hdr->dest_ip_addr = htonl(arp_hdr->dest_ip_addr);

    ethernet_hdr->FCS = 0;

    /*Dispatch ARP_BROADCAST_REQ pkt from out_intf*/
    send_pkt_out((char *)ethernet_hdr, ETH_HDR_SIZE_EXCL_PAYLOAD + payload_size, out_intf);
    free(ethernet_hdr);
}

static void send_arp_reply_msg(ethernet_hdr_t *ethernet_hdr_in, interface_t *out_intf)
{
    arp_hdr_t *arp_hdr_in = (arp_hdr_t *)(ethernet_hdr_in->payload);
    ethernet_hdr_t *ethernet_hdr_reply = (ethernet_hdr_t *)calloc(1, MAX_PKT_BUFF_SIZE);
    
    memcpy(ethernet_hdr_reply->dest_mac_addr.mac_addr, ethernet_hdr_in->src_mac_addr.mac_addr, sizeof(mac_addr_t));
    memcpy(ethernet_hdr_reply->src_mac_addr.mac_addr, IF_MAC(out_intf), sizeof(mac_addr_t));
    ethernet_hdr_reply->type = ARP_MSG;

    arp_hdr_t *arp_hdr_reply = (arp_hdr_t *)(ethernet_hdr_reply->payload);
    arp_hdr_reply->hw_type = 1;
    arp_hdr_reply->protocol_type = 0x0800;
    arp_hdr_reply->hw_addr_len = sizeof(mac_addr_t);
    arp_hdr_reply->protocol_addr_len = 4;

    arp_hdr_reply->op_code = ARP_REPLY;
    memcpy(arp_hdr_reply->src_mac_addr.mac_addr, IF_MAC(out_intf), sizeof(mac_addr_t));
    inet_pton(AF_INET, IF_IP(out_intf), &arp_hdr_reply->src_ip_addr);
    arp_hdr_reply->src_ip_addr = htonl(arp_hdr_reply->src_ip_addr);

    memcpy(arp_hdr_reply->dest_mac_addr.mac_addr, arp_hdr_in->src_mac_addr.mac_addr, sizeof(mac_addr_t));
    arp_hdr_reply->dest_ip_addr = arp_hdr_in->src_ip_addr;
    
    ethernet_hdr_reply->FCS = 0;

    unsigned int total_pkt_size = ETH_HDR_SIZE_EXCL_PAYLOAD + sizeof(arp_hdr_t);

    char *shifted_buff = pkt_buffer_shift_right((char *)ethernet_hdr_reply, total_pkt_size, MAX_PKT_BUFF_SIZE);

    send_pkt_out((char *)shifted_buff, total_pkt_size, out_intf);
    free(ethernet_hdr_reply);
}

static void process_arp_reply_msg(node_t *node, interface_t *in_intf, ethernet_hdr_t *ethernet_hdr)
{
    // printf(ANSI_COLOR_CYAN"%s : ARP Reply Msg RX on Intf : %s of Node : %s\n"ANSI_COLOR_RESET, 
                                                    // __FUNCTION__, in_intf->intf_name, in_intf->att_node->node_name);
    
    arp_table_entry_update_from_reply(node->node_nw_cfg.arp_table, (arp_hdr_t *)ethernet_hdr->payload, in_intf);
}

static void process_arp_broadcast_request(node_t *node, interface_t *in_intf, ethernet_hdr_t *ethernet_hdr)
{
    printf(ANSI_COLOR_GREEN"%s : ARP Broadcast Req RX on Intf : %s of Node : %s\n"ANSI_COLOR_RESET, 
                                                    __FUNCTION__, in_intf->intf_name, node->node_name);
    
    char ip_addr[16];
    printf("ARP BROADCAST MESSAGE RX ON NODE : %s\n", node->node_name);
    arp_hdr_t *arp_hdr = (arp_hdr_t *)(ethernet_hdr->payload);
    unsigned int arp_dest_ip = htonl(arp_hdr->dest_ip_addr);
    inet_ntop(AF_INET, &arp_dest_ip, ip_addr, 16);

    if(strncmp(IF_IP(in_intf), ip_addr, 15)) {
        printf(ANSI_COLOR_RED"%s : ARP Broadcast Msg Dropped, dest_ip_addr did not match\n"ANSI_COLOR_RESET,
                                                                                node->node_name);
        return;
    }
    send_arp_reply_msg(ethernet_hdr, in_intf);
}

//Entry Point into TCP-IP stack from the bottom
void  layer2_frame_rx(node_t *rx_node, interface_t *in_intf, char *pkt, unsigned int pkt_size)
{
    ethernet_hdr_t *ethernet_hdr = (ethernet_hdr_t *)pkt;

    if(layer2_frame_rx_qualifier(in_intf, ethernet_hdr) == FALSE) {
        printf(ANSI_COLOR_RED"ERROR : LAYER 2 FRAME REJECTED\n"ANSI_COLOR_RESET);
        return;
    }

    switch (ethernet_hdr->type) {
        case ARP_MSG: {

            arp_hdr_t *arp_hdr = (arp_hdr_t *)(ethernet_hdr->payload);
            switch (arp_hdr->op_code) {
                case ARP_BROADCAST_REQ: {

                    process_arp_broadcast_request(rx_node, in_intf, ethernet_hdr);
                    break;
                }
                case ARP_REPLY: {

                    process_arp_reply_msg(rx_node, in_intf, ethernet_hdr);    
                    break;
                }
                default:{
                    break;
                }
            }
            break;
        }
        default: {

            printf("RX MSG NOT A L2 MSG\n");
            break;
        }
    }

    return;
}

void init_arp_table(arp_table_t **arp_table)
{
    *arp_table = calloc(1, sizeof(arp_table_t));
    init_glthread(&((*arp_table)->arp_entries));
}

arp_entry_t *arp_table_entry_lookup(arp_table_t *arp_table, char *ip_addr)
{
    glthread_t *curr;
    arp_entry_t * arp_entry;

    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr) {

        arp_entry = arp_glue_to_arp_entry(curr);
        if(strncmp(arp_entry->ip_addr.ip_addr, ip_addr, IP_ADDR_SIZE)) {
            return arp_entry;
        }

    }ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr)

    return NULL;
}

void arp_table_entry_delete(arp_table_t *arp_table, char *ip_addr)
{
    arp_entry_t *arp_entry = arp_table_entry_lookup(arp_table, ip_addr);

    if(!arp_entry) {
        return;
    }

    remove_glthread(&arp_entry->arp_glue);
    free(arp_entry);
}

bool_e arp_table_entry_add(arp_table_t *arp_table, arp_entry_t *arp_entry)
{
    arp_entry_t *arp_entry_old = arp_table_entry_lookup(arp_table, arp_entry->ip_addr.ip_addr);

    if(arp_entry_old && IS_ARP_ENTRY_EQUAL(arp_entry_old, arp_entry)) {
        return FALSE;
        //in order to prevent mempry leak, if returned false, arp_entry must be freed
    }

    if(arp_entry_old) {
        arp_table_entry_delete(arp_table, arp_entry_old->ip_addr.ip_addr);
    }

    init_glthread(&arp_entry->arp_glue);
    glthread_add_next(&arp_table->arp_entries, &arp_entry->arp_glue);

    return TRUE;
}

void arp_table_entry_update_from_reply(arp_table_t *arp_table, arp_hdr_t *arp_hdr, interface_t *in_intf)
{
    unsigned int src_ip = 0;
    assert(arp_hdr->op_code == ARP_REPLY);

    arp_entry_t *arp_entry = calloc(1,sizeof(arp_entry_t));
    src_ip = htonl(arp_hdr->src_ip_addr);
    inet_ntop(AF_INET, &src_ip, (char *)&arp_entry->ip_addr.ip_addr, 16);
    arp_entry->ip_addr.ip_addr[15] = '\0';
    memcpy(arp_entry->mac_addr.mac_addr, arp_hdr->src_mac_addr.mac_addr, sizeof(mac_addr_t));
    strncpy(arp_entry->out_intf, in_intf->intf_name, INTF_NAME_SIZE);

    bool_e rc = arp_table_entry_add(arp_table, arp_entry);

    if(rc == FALSE) {
        printf("UNABLE TO ADD ARP ENTRY TO ARP TABLE\n");
        free(arp_entry);
    }
}

void dump_arp_table(arp_table_t *arp_table)
{
    glthread_t *curr;
    arp_entry_t *arp_entry;

    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr) {

        arp_entry = arp_glue_to_arp_entry(curr);
        printf("IP : %s\t MAC : %x:%x:%x:%x:%x:%x\t OUT_INTF : %s\n", arp_entry->ip_addr.ip_addr,
                                                                      arp_entry->mac_addr.mac_addr[0],
                                                                      arp_entry->mac_addr.mac_addr[1],
                                                                      arp_entry->mac_addr.mac_addr[2],
                                                                      arp_entry->mac_addr.mac_addr[3],
                                                                      arp_entry->mac_addr.mac_addr[4],
                                                                      arp_entry->mac_addr.mac_addr[5],
                                                                      arp_entry->out_intf);
    }ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr)
}