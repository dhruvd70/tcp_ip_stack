/*
* @Name :       utils.c
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/

#include "utils.h"
#include <arpa/inet.h>

void apply_mask(char *prefix, char mask, char *str_prefix)
{
    uint32_t binary_prefix = 0;
    uint32_t subnet_mask = 0xFFFFFFFF;

    if(mask == 32){
        strncpy(str_prefix, prefix, 16);
        str_prefix[15] = '\0';
        return;
    }

    inet_pton(AF_INET, prefix, &binary_prefix);
    binary_prefix = htonl(binary_prefix);

    subnet_mask = subnet_mask << (32 - mask);

    binary_prefix = binary_prefix & subnet_mask;

    binary_prefix = htonl(binary_prefix);
    inet_ntop(AF_INET, &binary_prefix, str_prefix, 16);
    str_prefix[15] = '\0';
}

void l2_fill_broadcast_mac(char *mac_arr)
{
    mac_arr[0] = 0xff;
    mac_arr[1] = 0xff;
    mac_arr[2] = 0xff;
    mac_arr[3] = 0xff;
    mac_arr[4] = 0xff;
    mac_arr[5] = 0xff;
}