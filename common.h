/*
* @Name :       common.h
* @author:      Dhruv Dohare(dhruv.dohare04@gmail.com)
* 
*/
#ifndef __COMMON__
#define __COMMON__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum bool_{
    FALSE,
    TRUE
}bool_e;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

#define ANSI_COLOR_RESET   "\x1b[0m"

#endif //__COMMON__