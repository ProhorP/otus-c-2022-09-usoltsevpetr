#include "liblog.h"
int lib_log_fd = 0;
char *lib_log_msg = NULL;
unsigned int lib_log_msg_size = 5;


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

  if (!(lib_log_msg = (char *) malloc (lib_log_msg_size)))
    {
      perror ("Ошибка выделения памяти\n");
      return LIB_LOG_INIT_ERROR_MALLOC;

    }

  *lib_log_msg = '\0';

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

  free (lib_log_msg);

  return LIB_LOG_CLOSE_SUCCESS;

}

void
lib_log_append (char *message, ...)
{

  unsigned int length = 0;
  char *ptr = message;
  va_list argptr;

  va_start (argptr, message);
/*считаем общую длину всех параметров*/
  while (ptr != NULL)
    {
      length += strlen (ptr);
      ptr = va_arg (argptr, char *);
    }

  /*символ перевода строки и окончания строки тоже учитываем*/
  length += 2;

  va_end (argptr);

  if (length > lib_log_msg_size)
    {

      lib_log_msg_size = max (length, lib_log_msg_size << 1);

      if (!(lib_log_msg = (char *) realloc (lib_log_msg, lib_log_msg_size)))
	{

	  //Блок ошибки выделения памяти
	  char *error_msg = "Ошибка выделения памяти\n";

	  perror (error_msg);

	  if (write (lib_log_fd, error_msg, strlen (error_msg)) !=
	      (int) strlen (error_msg))
	    perror ("Не записаны элементы");

	  if (close (lib_log_fd) < 0)
	    perror
	      ("Ошибка закрытия файла логирования");

	  exit (1);

	}

      *lib_log_msg = '\0';

    }

  /*запишем все тестовые параметры в одну строку*/
  va_start (argptr, message);
  ptr = message;
  length = 0;

  while (ptr != NULL)
    {
      memcpy (lib_log_msg + length, ptr, strlen (ptr));
      length += strlen (ptr);
      ptr = va_arg (argptr, char *);
    }

  va_end (argptr);

  memcpy (lib_log_msg + length, "\n\0", 2);

  /*ошибка записи в файл это плохо, но это не поломает программу
   * поэтому не буду прекращать программу при ошибке записи в файл*/
  if (write (lib_log_fd, lib_log_msg, strlen (lib_log_msg)) !=
      (int) strlen (lib_log_msg))
    perror ("Не записаны элементы");


}
