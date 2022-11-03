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
typedef CURL *(type_fptr_curl_easy_init) (void);
typedef void (type_fptr_curl_easy_cleanup) (CURL * handle);
typedef CURLcode (type_fptr_curl_easy_setopt) (CURL * handle,
					       CURLoption option,
					       void *param);
typedef CURLcode (type_fptr_curl_easy_perform) (CURL * easy_handle);

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

  /*Получаем указатели на функции из подключаемой библиотеки */

  type_fptr_curl_version_info *fptr_curl_version_info =
    (type_fptr_curl_version_info *) dlsym (dyn_lib, "curl_version_info");

  if (!fptr_curl_version_info)
    error_exit
      ("Не удалось найти функцию curl_version_info");

  type_fptr_curl_global_init *fptr_curl_global_init =
    (type_fptr_curl_global_init *) dlsym (dyn_lib, "curl_global_init");

  if (!fptr_curl_global_init)
    error_exit
      ("Не удалось найти функцию curl_global_init");

  type_fptr_curl_global_cleanup *fptr_curl_global_cleanup =
    (type_fptr_curl_global_cleanup *) dlsym (dyn_lib, "curl_global_cleanup");

  if (!fptr_curl_global_cleanup)
    error_exit
      ("Не удалось найти  функцию curl_global_cleanup\n");

  type_fptr_curl_easy_init *fptr_curl_easy_init =
    (type_fptr_curl_easy_init *) dlsym (dyn_lib, "curl_easy_init");

  if (!fptr_curl_easy_init)
    error_exit
      ("Не удалось найти функцию curl_easy_init");

  type_fptr_curl_easy_cleanup *fptr_curl_easy_cleanup =
    (type_fptr_curl_easy_cleanup *) dlsym (dyn_lib, "curl_easy_cleanup");

  if (!fptr_curl_easy_cleanup)
    error_exit
      ("Не удалось найти функцию curl_easy_cleanup");

  type_fptr_curl_easy_setopt *fptr_curl_easy_setopt =
    (type_fptr_curl_easy_setopt *) dlsym (dyn_lib, "curl_easy_setopt");

  if (!fptr_curl_easy_setopt)
    error_exit
      ("Не удалось найти функцию curl_easy_setopt");

  type_fptr_curl_easy_perform *fptr_curl_easy_perform =
    (type_fptr_curl_easy_perform *) dlsym (dyn_lib, "curl_easy_setopt");

  if (!fptr_curl_easy_perform)
    error_exit
      ("Не удалось найти функцию curl_easy_perform");

  /*Начинаем работу */

  /*получаем данные о версии curl из динамической бибилиотеки и выводим их */
  curl_version_info_data *curl_about =
    fptr_curl_version_info (CURLVERSION_NOW);

  if (!curl_about)
    error_exit
      ("Функция curl_version_info вернула пустой указатель\n");

  printf ("Используется версия curl: %s\n",
	  curl_about->version);

  /*Делаем инициализацию */

  if (fptr_curl_global_init (CURL_GLOBAL_ALL))
    error_exit
      ("Инициализация curl завершилась с ошибкой");

  /*Получаем простой интерфейс */

  CURL *curl = fptr_curl_easy_init ();

  if (!curl)
    error_exit
      ("Не удалось получить простой интерфейс curl");

  CURLcode res;
  char errbuf[CURL_ERROR_SIZE];

  if ((res =
       fptr_curl_easy_setopt (curl, CURLOPT_URL,
			      "https://example.com")) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_URL привела к результату %d",
       (int) res);

  if ((res =
       fptr_curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf)) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_ERRORBUFFER привела к результату %d",
       (int) res);

  if ((res =
       fptr_curl_easy_setopt (curl, CURLOPT_VERBOSE, 1)) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_VERBOSE привела к результату %d",
       (int) res);


  /* set the error buffer as empty before performing a request */
  errbuf[0] = 0;

  if ((res = fptr_curl_easy_perform (curl)) != CURLE_OK)
      ("Код возврата при доступе на сайт = %d. %s\n",
       (int) res, errbuf);

  /*Очистка простого интерфейса не возвращает результата */
  fptr_curl_easy_cleanup (curl);

  /*Отмена глобальной инициализации */
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
