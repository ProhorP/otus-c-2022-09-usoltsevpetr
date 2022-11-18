#include "liblog.h"

int
main (int argc, char *argv[])
{


  if (argc != 2)
    {
      printf ("Неверно введена команда\n"
	      "Нужно ввести в таком формате:\n"
	      "\"./test name_log_file\"\n");
      return EXIT_FAILURE;
    }


  if (lib_log_init (argv[1]) != LIB_LOG_INIT_SUCCESS)
    return EXIT_FAILURE;

  lib_log_append ("message1", "warning", NULL);

  if (lib_log_close () != LIB_LOG_CLOSE_SUCCESS)
    return EXIT_FAILURE;


  return 0;

}

