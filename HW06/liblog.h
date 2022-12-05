#include <stdbool.h>

#ifndef _LIB_LOG_
#define _LIB_LOG_

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)
#define LIB_LOG_FORMAT_PRINT "File \"%s\" func %s:%s "
#define LIB_LOG_END_STRING "\n"
#define LIB_LOG_BACKTRACE "стек вызовов:"
#define LIB_LOG_ERROR "ERROR "
#define LIB_LOG_WARNING "WARNING "
#define LIB_LOG_INFO "INFO "

#define lib_log_error(format, ...) \
	lib_log_append(true,\
			LIB_LOG_ERROR LIB_LOG_FORMAT_PRINT # format LIB_LOG_END_STRING LIB_LOG_BACKTRACE LIB_LOG_END_STRING,\
			__FILE__,__func__,S__LINE__,__VA_ARGS__)

#define lib_log_warning(format, ...) \
	lib_log_append(false,\
			LIB_LOG_WARNING LIB_LOG_FORMAT_PRINT # format LIB_LOG_END_STRING,\
			__FILE__,__func__,S__LINE__,__VA_ARGS__)

#define lib_log_info(format, ...) \
	lib_log_append(false,\
			LIB_LOG_INFO LIB_LOG_FORMAT_PRINT # format LIB_LOG_END_STRING,\
			__FILE__,__func__,S__LINE__,__VA_ARGS__)

typedef enum
{
  LIB_LOG_INIT_SUCCESS = 0,
  LIB_LOG_INIT_ERROR_OPEN = -1,
} lib_log_init_ret;

typedef enum
{
  LIB_LOG_CLOSE_SUCCESS = 0,
  LIB_LOG_CLOSE_ERROR_CLOSE = -1
} lib_log_close_ret;

lib_log_init_ret lib_log_init (char *name_log_file);
lib_log_close_ret lib_log_close (void);
void lib_log_append (bool print_backtrace, char *format, ...);
#endif
