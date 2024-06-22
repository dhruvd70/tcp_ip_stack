/*
* @Name :       nwcli.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/
#include "Command_Parser/libcli.h"
#include "Command_Parser/cmdtlv.h"
#include "graph/graph.h"
#include "cmdcodes.h"

#include "common.h"

extern graph_t *topo;

static int show_nw_topo_handler(param_t* param, ser_buff_t *tlv_buf, 
                                op_mode enable_or_disable)
{
    int CMD_CODE = -1;
    CMD_CODE = EXTRACT_CMD_CODE(tlv_buf);

    switch(CMD_CODE) {
    case CMD_CODE_SHOW_NW_TOPOLOGY:{
        dump_graph(topo);
    }
        break;
    
    default:
        break;
    }
}

/*LAYER -2*/

typedef struct arp_table_ arp_table_t;

extern void dump_arp_table(arp_table_t *arp_table);

static int show_arp_handler(param_t* param, ser_buff_t *tlv_buf, op_mode enable_or_disable)
{
    node_t *node;
    char *node_name;
    tlv_struct_t *tlv = NULL;

    TLV_LOOP_BEGIN(tlv_buf, tlv) {

        if(strncmp(tlv->leaf_id, "node-name", sizeof("node-name")) == 0) {
            node_name = tlv->value;
        }
    }TLV_LOOP_END;

    node = get_node_by_node_name(topo, node_name);
    dump_arp_table(node->node_nw_cfg.arp_table);
    return 0;
}

extern void send_arp_broadcast_request(node_t *node, interface_t *out_intf, char *ip_addr);

static int arp_handler(param_t* param, ser_buff_t *tlv_buf, op_mode enable_or_disable)
{
    node_t *node;
    char *node_name;
    char *ip_addr;
    tlv_struct_t *tlv = NULL;

    TLV_LOOP_BEGIN(tlv_buf, tlv) {

        if(strncmp(tlv->leaf_id, "node-name", sizeof("node-name")) == 0) {
            node_name = tlv->value;
        }
        else if(strncmp(tlv->leaf_id, "ip-address", sizeof("ip-address")) == 0) {
            ip_addr = tlv->value;
        }
    }TLV_LOOP_END;

    node = get_node_by_node_name(topo, node_name);
    send_arp_broadcast_request(node, NULL, ip_addr);
    return 0;
}

void nw_init_cli()
{
    init_libcli();

    param_t* show       = libcli_get_show_hook();
    param_t* debug      = libcli_get_debug_hook();
    param_t* config     = libcli_get_config_hook();
    param_t* run        = libcli_get_run_hook();
    param_t* debug_show = libcli_get_debug_show_hook();
    param_t* root       = libcli_get_root();

    {
        /*show topology*/
        static param_t topology;
        init_param(&topology, CMD, "topology", show_nw_topo_handler, 0, INVALID,
                    0, "Dump Complete Network Topology");
        libcli_register_param(show, &topology);
        set_param_cmd_code(&topology, CMD_CODE_SHOW_NW_TOPOLOGY);
    }
    support_cmd_negation(config);
}