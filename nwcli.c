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