#include <stdio.h>
#include <stdlib.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
#include <dlfcn.h>

void error_exit (char *str);
void *dlopen (const char *filename, int flags);

int
main (int argc, char *argv[])
{

  if (argc != 2)
    {
      error_exit ("Неверно введена команда\n"
		  "Нужно ввести в таком формате:\n"
		  "\"./weather city_name\"\n");
    }

  /* подгрузка библиотеки */

  void *dyn_lib = dlopen ("libcurl.so", RTLD_LAZY | RTLD_GLOBAL);

/* проверка на успешность */

  if (!dyn_lib)
    error_exit
      ("Не удалось подключить динамическую бибилиотеку libcurl.so, не найдена в системe\n");

  /* выгружаем библиотеку */
  dlclose (dyn_lib);

  return 0;

}

void
error_exit (char *str)
{

  printf ("%s\n", str);
  exit (1);

}
