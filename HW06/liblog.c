#include "liblog.h"

int lib_log_fd = 0;

lib_log_init_ret
lib_log_init (char *name_log_file)
{

  if ((lib_log_fd =
       open (name_log_file, O_WRONLY | O_CREAT | O_APPEND,
	     S_IWUSR | S_IRUSR)) < 0)
    {
      perror ("Ошибка инициализации журнала\n");
      return LIB_LOG_INIT_ERROR_OPEN;
    }

  return LIB_LOG_INIT_SUCCESS;

}

lib_log_close_ret
lib_log_close (void)
{

  if (close (lib_log_fd) < 0)
    {
      perror
	("Ошибка закрытия файла логирования");
      return LIB_LOG_CLOSE_ERROR_CLOSE;
    }

  return LIB_LOG_CLOSE_SUCCESS;

}

void
lib_log_append (bool print_backtrace, char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
  if (vdprintf (lib_log_fd, format, argptr) < 0)
    perror ("Не записаны элементы");
#pragma GCC diagnostic pop

  va_end (argptr);

  if (print_backtrace)
    {
      int nptrs;
      void *buffer[BT_BUF_SIZE];
      nptrs = backtrace (buffer, BT_BUF_SIZE);
      backtrace_symbols_fd (buffer, nptrs, lib_log_fd);

    }

}
