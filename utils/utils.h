/*
* @Name :       utils.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/


#ifndef __UTILS__
#define __UTILS__

#include "common.h"

#define IS_MAC_BROADCAST_ADDR(mac)  \
    (mac[0] == 0xFF && mac[1] == 0xFF && mac[2] == 0xFF && mac[3] == 0xFF && mac[4] == 0xFF && mac[5] == 0xFF)

void apply_mask(char *prefix, char mask, char *str_prefix);

void l2_fill_broadcast_mac(char *mac_arr);

#endif