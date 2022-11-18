#ifndef _LIB_LOG_
#define _LIB_LOG_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

typedef enum
{
  LIB_LOG_INIT_SUCCESS = 0,
  LIB_LOG_INIT_ERROR_OPEN = -1,
  LIB_LOG_INIT_ERROR_MALLOC = -2
} lib_log_init_ret;

typedef enum
{
  LIB_LOG_CLOSE_SUCCESS = 0,
  LIB_LOG_CLOSE_ERROR_CLOSE = -1
} lib_log_close_ret;

lib_log_init_ret lib_log_init (char *name_log_file);
lib_log_close_ret lib_log_close (void);
void lib_log_append (char *message, ...);

#endif
