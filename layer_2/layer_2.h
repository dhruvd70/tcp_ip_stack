/*
* @Name :       layer_2.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#ifndef __LAYER_2__
#define __LAYER_2__

#include "../utils/utils.h"
#include "../graph/net_params.h"
#include "../graph/graph.h"
#include "../gluethread/glthread.h"

#pragma pack(push,1) //to prevent padding bytes added by compiler

typedef struct ethernet_hdr_{
    mac_addr_t dest_mac_addr;
    mac_addr_t src_mac_addr;
    short int type;
    char payload[248];
    unsigned int FCS;    
}ethernet_hdr_t;

#pragma pack(pop)

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
