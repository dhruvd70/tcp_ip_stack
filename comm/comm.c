/*
* @Name :       comm.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "comm.h"
// #include "../layer_2/layer_2.h"

static unsigned int udp_port_number = 40000;

static char rx_buff[MAX_PKT_BUFF_SIZE];
static char tx_buff[MAX_PKT_BUFF_SIZE];

unsigned int get_next_udp_port_number()
{
    return udp_port_number++;
}

void print_buff(char *buff, int len)
{
    for(int i=0;i<len;i++) {
        printf("%c",buff[i]);
    }
    printf("\n");
}

// extern void layer2_frame_rx(node_t *rx_node, interface_t *intf, char *pkt, unsigned int pkt_size);

//Entry point into data link layer from physical layer
int pkt_rx(node_t* rx_node, interface_t* intf, char* pkt,
                    unsigned int pkt_size)
{
    // printf("MSG RX = %s, NODE = %s, INTF = %s\n", pkt, rx_node->node_name, intf->intf_name);
    pkt = pkt_buffer_shift_right(pkt, pkt_size, MAX_PKT_BUFF_SIZE - INTF_NAME_SIZE);

    layer2_frame_rx(rx_node, intf, pkt, pkt_size);
    return 0;
}

static void _pkt_rx(node_t* rx_node, char* pkt_w_aux_data, unsigned int pkt_size)
{
    char* rx_intf_name = pkt_w_aux_data;
    interface_t* rx_intf = get_node_intf_by_name(rx_node, rx_intf_name);

    if(!rx_intf) {
        printf("ERROR: PKT RX ON UNKNOWN INTERFACE %s ON NODE %s\n",
                                                        rx_intf->intf_name, 
                                                        rx_node->node_name);
        return;
    }
    pkt_rx(rx_node, rx_intf, pkt_w_aux_data + INTF_NAME_SIZE, 
            pkt_size - INTF_NAME_SIZE);
}

static int _send_pkt_out(int sock_fd, char* pkt_data, unsigned int pkt_size, 
                        unsigned int dest_port_number)
{
    int rc = 0;
    struct sockaddr_in dest_addr;

    struct hostent* host= (struct hostent*)gethostbyname("127.0.0.1");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port   = dest_port_number;
    dest_addr.sin_addr.s_addr  = inet_addr("127.0.0.1");

    rc = sendto(sock_fd, pkt_data, pkt_size, 0, (struct sockaddr *)&dest_addr, 
                sizeof(struct sockaddr));
    return rc;
}

int send_pkt_out(char* pkt, unsigned int pkt_size, interface_t* intf)
{
    int rc = 0;
    node_t* sending_node = intf->att_node;
    node_t* nbr_node = get_nbr_node(intf);

    if(!nbr_node) {
        return -1;
    }
    unsigned int dest_udp_port = nbr_node->udp_port_number;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0) {
        printf("ERROR: SENDING SOCKET CREATION FAILED, error no. = %d\n",errno);
    }
    interface_t* other_intf = &intf->link->intf_1 == intf ? \
                                &intf->link->intf_2 : &intf->link->intf_1;
    
    memset(tx_buff, 0, MAX_PKT_BUFF_SIZE);

    char* pkt_w_aux_data = tx_buff;
    strncpy(pkt_w_aux_data, other_intf->intf_name, INTF_NAME_SIZE);
    pkt_w_aux_data[INTF_NAME_SIZE] = '\0';
    memcpy(pkt_w_aux_data + INTF_NAME_SIZE, pkt, pkt_size);
    rc = _send_pkt_out(sock, pkt_w_aux_data, pkt_size + INTF_NAME_SIZE, 
                                                        dest_udp_port);

    close(sock);
    return rc;
}

void init_udp_socket(node_t* node)
{
    node->udp_port_number = get_next_udp_port_number();
    int udp_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in node_addr;
    node_addr.sin_family        = AF_INET;
    node_addr.sin_port          = node->udp_port_number;
    node_addr.sin_addr.s_addr   = INADDR_ANY;

    if(bind(udp_sock_fd, (struct sockaddr*)&node_addr, 
        sizeof(struct sockaddr)) == -1) {
        printf("ERROR: SOCKET BIND FAILED FOR NODE %s\n", node->node_name);
        return;
    }
    node->udp_socket_fd = udp_sock_fd;
}

static void* rx_pkt_thread_func(void* arg)
{
    node_t* node;
    glthread_t* curr;

    fd_set active_sock_fd_set, backup_sock_fd_set;

    int sock_max_fd = 0;
    int bytes_rx    = 0;
    graph_t* topo = (void *)arg;
    int addr_len = sizeof(struct sockaddr);

    FD_ZERO(&active_sock_fd_set);
    FD_ZERO(&backup_sock_fd_set);

    struct sockaddr_in sender_addr;

    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr) {
        node = graph_glue_to_node(curr);
        if(!node->udp_socket_fd) {
            continue;
        }
        if(node->udp_socket_fd > sock_max_fd) {
            sock_max_fd = node->udp_socket_fd; 
        }
        FD_SET(node->udp_socket_fd, &backup_sock_fd_set);
    }ITERATE_GLTHREAD_END(&topo->node_list, curr);

    while(1) {
        memcpy(&active_sock_fd_set, &backup_sock_fd_set, sizeof(fd_set));
        select(sock_max_fd + 1, &active_sock_fd_set, NULL, NULL, NULL);

        ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr) {
            node = graph_glue_to_node(curr);

            if(FD_ISSET(node->udp_socket_fd, &active_sock_fd_set)) {
                memset(rx_buff, 0, MAX_PKT_BUFF_SIZE);
                bytes_rx = recvfrom(node->udp_socket_fd, (char*)rx_buff, 
                                    MAX_PKT_BUFF_SIZE, 0,
                                    (struct sockaddr *)&sender_addr, &addr_len);
                _pkt_rx(node, rx_buff, bytes_rx);
            }
        }ITERATE_GLTHREAD_END(&topo->node_list, curr);
    }
}

void network_start_pkt_rx_thread(graph_t* topo)
{
    pthread_attr_t attribute;
    pthread_t rx_pkt_thread;
    pthread_attr_init(&attribute);
    pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);

    pthread_create(&rx_pkt_thread, &attribute, rx_pkt_thread_func, (void*)topo);
}