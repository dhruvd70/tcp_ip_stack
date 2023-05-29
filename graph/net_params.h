/*
* @Name :       net.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* @Discription: 
* 
*/

#include "../common.h"

#define IP_ADDR_SIZE        16
#define MAC_ADDR_SIZE        6


typedef struct ip_addr_
{
    char ip_addr[IP_ADDR_SIZE];
}ip_addr_t;

typedef struct mac_addr_
{
    char mac_addr[MAC_ADDR_SIZE];
}mac_addr_t;

typedef struct node_nw_prop_
{
    unsigned int hw_type;
    /*L3 Property*/
    bool b_lb_addr_cfg;         /*true if lb addr is present*/
    ip_addr_t lb_ip_addr;
}node_nw_prop_t;

typedef struct intf_nw_prop_
{
    /*L2 Property*/
    mac_addr_t mac_addr;

    /*L3 Property*/
    bool b_ip_addr_cfg;         /*true if ip addr is present*/
    ip_addr_t ip_addr;
    char mask_val;
}intf_nw_prop_t;

static void init_nw_intf_prop(intf_nw_prop_t *intf_nw_prop)
{
    memset(intf_nw_prop->mac_addr.mac_addr, 0, MAC_ADDR_SIZE);
    intf_nw_prop->b_ip_addr_cfg = false;
    memset(intf_nw_prop->ip_addr.ip_addr, 0, IP_ADDR_SIZE);
    intf_nw_prop->mask_val = 0;
}