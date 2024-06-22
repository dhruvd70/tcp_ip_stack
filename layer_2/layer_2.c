/*
* @Name :       layer_2.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "layer_2.h"
#include "../tcp_const.h"
#include <arpa/inet.h>

//Entry Point into TCP-IP stack from the bottom
void  layer2_frame_rx(node_t *rx_node, interface_t *intf, char *pkt, unsigned int pkt_size)
{
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

    if(arp_entry_old && memcmp(arp_entry_old, arp_entry, sizeof(arp_entry_t))) {
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
        free(arp_entry);
    }
}

void dump_arp_table(arp_table_t *arp_table)
{
    glthread_t *curr;
    arp_entry_t *arp_entry;

    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr) {

        arp_entry = arp_glue_to_arp_entry(curr);
        printf("IP : %s\t MAC : %u:%u:%u:%u:%u:%u\t OUT_INTF : %s\n", arp_entry->ip_addr.ip_addr,
                                                                      arp_entry->mac_addr.mac_addr[0],
                                                                      arp_entry->mac_addr.mac_addr[1],
                                                                      arp_entry->mac_addr.mac_addr[2],
                                                                      arp_entry->mac_addr.mac_addr[3],
                                                                      arp_entry->mac_addr.mac_addr[4],
                                                                      arp_entry->mac_addr.mac_addr[5],
                                                                      arp_entry->out_intf);
    }ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr)
}

void send_arp_broadcast_request(node_t *node, interface_t *out_intf, char *ip_addr)
{
    return;
}