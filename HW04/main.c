#include <stdio.h>
#include <stdlib.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
#include <dlfcn.h>
#include <curl/curl.h>

typedef CURLcode (type_fptr_curl_global_init) (long flags);
typedef void (type_fptr_curl_global_cleanup) (void);
typedef curl_version_info_data *(type_fptr_curl_version_info) (CURLversion
							       age);

void error_exit (char *str);

int
main (int argc, char *argv[])
{

  if (argc != 2)
    {
      error_exit ("Неверно введена команда\n"
		  "Нужно ввести в таком формате:\n"
		  "\"./weather city_name\"\n");
    }

  printf ("Погода для %s:\n", argv[1]);

  /* подгрузка библиотеки */

  void *dyn_lib = dlopen ("libcurl.so", RTLD_LAZY | RTLD_GLOBAL);

  /* проверка на успешность */

  if (!dyn_lib)
    error_exit
      ("Не удалось подключить динамическую бибилиотеку libcurl.so, не найдена в системe\n");

  /*получаем данные о версии curl из динамической бибилиотеки и выводим их */

  type_fptr_curl_version_info *fptr_curl_version_info =
    (type_fptr_curl_version_info *) dlsym (dyn_lib, "curl_version_info");

  if (!fptr_curl_version_info)
    error_exit
      ("Не удалось найти функцию curl_version_info");

  curl_version_info_data *curl_about =
    fptr_curl_version_info (CURLVERSION_NOW);

  if (!curl_about)
	  error_exit("Функция curl_version_info вернула пустой указатель\n");

  printf ("Используется версия curl: %s\n",
	  curl_about->version);

  /*Делаем инициализацию */

  type_fptr_curl_global_init *fptr_curl_global_init =
    (type_fptr_curl_global_init *) dlsym (dyn_lib, "curl_global_init");

  if (!fptr_curl_global_init)
    error_exit
      ("Не удалось найти функцию curl_global_init");

  if (fptr_curl_global_init (CURL_GLOBAL_ALL))
    error_exit
      ("Инициализация curl завершилась с ошибкой");

  /*Отмена глобальной инициализации */
  type_fptr_curl_global_cleanup *fptr_curl_global_cleanup =
    (type_fptr_curl_global_cleanup *) dlsym (dyn_lib, "curl_global_cleanup");

  if (!fptr_curl_global_cleanup)
    error_exit
      ("Не удалось найти  функцию curl_global_cleanup\n");

  /*Вызываем curl_global_cleanup без проверки успешности
   * т.к. функция не принимает параметры и ничего не возвращает*/
  fptr_curl_global_cleanup ();

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
