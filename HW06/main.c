#include "liblog.h"
#include <stdio.h>
#include <stdlib.h>

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

  lib_log_info ("%s%s", "message1", "message2");
  lib_log_warning ("%d%d", 555, 666);
  lib_log_error ("%s%s", "message3", "message4");

  if (lib_log_close () != LIB_LOG_CLOSE_SUCCESS)
    return EXIT_FAILURE;


  return EXIT_SUCCESS;

}
