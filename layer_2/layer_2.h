/*
* @Name :       layer_2.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#ifndef __LAYER_2__
#define __LAYER_2__

#include "../utils/utils.h"
#include "../graph/graph.h"
#include "../gluethread/glthread.h"

#pragma pack(push,1) //to prevent padding bytes added by compiler

typedef struct arp_hdr_{
    short hw_type;              // 1 for ethernet 
    short protocol_type;        // 0x0800 for IPv4
    char hw_addr_len;           // 6-bytes for MAC
    char protocol_addr_len;     // 4-bytes for IPv4
    short op_code;              // 1->REQ : 2->REPLY
    mac_addr_t src_mac_addr;    // MAC of OIF
    unsigned int src_ip_addr;   // IP of OIF
    mac_addr_t dest_mac_addr;   // MAC of dest node or broadcast
    unsigned int dest_ip_addr;  // IP for which ARP is being resolved
}arp_hdr_t;

typedef struct ethernet_hdr_{
    mac_addr_t dest_mac_addr;
    mac_addr_t src_mac_addr;
    unsigned short type;
    char payload[248];
    unsigned int FCS;    
}ethernet_hdr_t;

#pragma pack(pop)

typedef struct arp_table_ {

    glthread_t arp_entries;
}arp_table_t;;


typedef struct arp_entry_ {

    ip_addr_t ip_addr; //KEY
    mac_addr_t mac_addr;
    char out_intf[INTF_NAME_SIZE];
    glthread_t glue;
}arp_entry_t;


#define ETH_HDR_SIZE_EXCL_PAYLOAD (sizeof(ethernet_hdr_t) - sizeof(((ethernet_hdr_t *)0)->payload))

#define ETH_FCS(eth_hdr_ptr, payload_size)  \
    (*(unsigned int *)(((char *)(((ethernet_hdr_t *)eth_hdr_ptr)->payload) + payload_size)))



static ethernet_hdr_t* alloc_eth_hdr_with_payload(char *pkt, unsigned int pkt_size)
{
    char *temp = (char *)calloc(1, sizeof(ethernet_hdr_t));
    memcpy(temp, pkt, pkt_size);

    ethernet_hdr_t *eth_hdr = (ethernet_hdr_t *)(pkt - ETH_HDR_SIZE_EXCL_PAYLOAD);
    memset((char *)eth_hdr, 0, ETH_HDR_SIZE_EXCL_PAYLOAD);
    memcpy(eth_hdr->payload, temp, pkt_size);
    //set FCS
    free(temp);
    return eth_hdr;
}

static inline bool_e l2_frame_qualifier_on_interface(interface_t *intf, ethernet_hdr_t *eth_hdr)
{
    if(!IS_INTF_L3_MODE(intf)) {
        return FALSE;
    }

    //accept if dest MAC addr matches with MAC addr of intf
    if(memcmp(intf->intf_nw_cfg.mac_addr.mac_addr,
              eth_hdr->dest_mac_addr.mac_addr, 
              sizeof(mac_addr_t)) == 0) {
        return TRUE;
    }

    if(IS_MAC_BROADCAST_ADDR(eth_hdr->dest_mac_addr.mac_addr)) {
        return TRUE;
    }

    return FALSE;
}



#endif // __LAYER_2__
