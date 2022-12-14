#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
#include <dlfcn.h>
#include <curl/curl.h>
#include "parson/parson.h"

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
typedef struct curl_slist *(type_fptr_curl_slist_append) (struct curl_slist *
							  list,
							  const char *string);
typedef void (type_fptr_curl_slist_free_all) (struct curl_slist * list);

struct memory
{
  char *response;
  size_t size;
};

void error_exit (char *str);
static size_t cb (void *data, size_t size, size_t nmemb, void *userp);

int
main (int argc, char *argv[])
{

  if (argc != 3)
    {
      error_exit ("Неверно введена команда\n"
		  "Нужно ввести в таком формате:\n"
		  "\"./weather широта(например 55.75396) долгота(например 37.620393)\"\n");
    }

  printf
    ("Погода для адреса с широтой %s и долготой %s:\n",
     argv[1], argv[2]);

  char url[100] = { 0 };

  sprintf (url,
	   "https://api.weather.yandex.ru/v2/forecast?lat=%s&lon=%s&extra=false",
	   argv[1], argv[2]);

  /* подгрузка библиотеки */

  void *dyn_lib = dlopen ("libcurl.so", RTLD_LAZY | RTLD_GLOBAL);

  /* проверка на успешность */

  if (!dyn_lib)
    error_exit
      ("Не удалось подключить динамическую бибилиотеку libcurl.so, не найдена в системe\n");

  /*Получаем указатели на функции из подключаемой библиотеки */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

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
    (type_fptr_curl_easy_perform *) dlsym (dyn_lib, "curl_easy_perform");

  if (!fptr_curl_easy_perform)
    error_exit
      ("Не удалось найти функцию curl_easy_perform");

  type_fptr_curl_slist_append *fptr_curl_slist_append =
    (type_fptr_curl_slist_append *) dlsym (dyn_lib, "curl_slist_append");

  if (!fptr_curl_slist_append)
    error_exit
      ("Не удалось найти функцию curl_slist_append");

  type_fptr_curl_slist_free_all *fptr_curl_slist_free_all =
    (type_fptr_curl_slist_free_all *) dlsym (dyn_lib, "curl_slist_free_all");

  if (!fptr_curl_slist_free_all)
    error_exit
      ("Не удалось найти функцию curl_slist_free_all");

#pragma GCC diagnostic pop

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

  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;

  if ((res = fptr_curl_easy_setopt (curl, CURLOPT_URL, url)) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_URL привела к результату %d",
       (int) res);

  if ((res =
       fptr_curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf)) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_ERRORBUFFER привела к результату %d",
       (int) res);

  int value_1 = 1;

  if ((res =
       fptr_curl_easy_setopt (curl, CURLOPT_VERBOSE,
			      (int *) &value_1)) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_VERBOSE привела к результату %d",
       (int) res);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
  if ((res =
       fptr_curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, cb)) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_WRITEDATA привела к результату %d",
       (int) res);
#pragma GCC diagnostic pop

  struct memory chunk = { 0 };

  if ((res =
       fptr_curl_easy_setopt (curl, CURLOPT_WRITEDATA,
			      (void *) &chunk)) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_WRITEDATA привела к результату %d",
       (int) res);

  struct curl_slist *list = NULL;

  list =
    fptr_curl_slist_append (list,
			    "X-Yandex-API-Key: 5b2f9374-6470-4f19-8d29-e5e27adf5d04");

  if ((res =
       fptr_curl_easy_setopt (curl, CURLOPT_HTTPHEADER, list)) != CURLE_OK)
    printf
      ("Установка параметра CURLOPT_HTTPHEADER привела к результату %d",
       (int) res);


  /* set the error buffer as empty before performing a request */
  errbuf[0] = 0;


  if ((res = fptr_curl_easy_perform (curl)) != CURLE_OK)
    printf
      ("Код возврата при доступе на сайт = %d. %s\n",
       (int) res, errbuf);

  /*Освобождение памяти выделенной для переменной curl_slist
   * (использова для передачи заголовка)*/

  fptr_curl_slist_free_all (list);


  /*Очистка простого интерфейса не возвращает результата */
  fptr_curl_easy_cleanup (curl);

  /*Отмена глобальной инициализации */
  /*Вызываем curl_global_cleanup без проверки успешности
   * т.к. функция не принимает параметры и ничего не возвращает*/
  fptr_curl_global_cleanup ();

  /* выгружаем библиотеку */

  dlclose (dyn_lib);

  /*парсим json библиотекой parson */

  JSON_Value *root_value = json_parse_string (chunk.response);

  JSON_Object *evening_object =
    json_object_get_object (json_object_get_object
			    (json_array_get_object
			     (json_object_get_array
			      (json_value_get_object (root_value),
			       "forecasts"), 0), "parts"), "evening");

/* Тестовая строка json для проверки библиотеки
 * структура json согласно API Yandex из примеров на сайте
  char *str = "{\"forecasts\":[{\"parts\":{\"evening\":{\"condition\":\"rain\",\"temp_min\":25,\"temp_max\":30,\"wind_speed\":1.8,\"wind_dir\":\"nw\"}}}]}";
  */

  printf ("Описание погоды: %s\n",
	  json_object_get_string (evening_object, "condition"));

  printf ("Температура в диапазоне от %d до %d\n",
	  (int) json_object_get_number (evening_object, "temp_min"),
	  (int) json_object_get_number (evening_object, "temp_max"));

  printf ("Скорость ветра %0.1f, направление %s\n",
	  (float) json_object_get_number (evening_object, "wind_speed"),
	  json_object_get_string (evening_object, "wind_dir"));

  json_value_free (root_value);

  free (chunk.response);

  return 0;

}

void
error_exit (char *str)
{

  printf ("%s\n", str);
  exit (1);

}

static size_t
cb (void *data, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *) userp;

  char *ptr = realloc (mem->response, mem->size + realsize + 1);
  if (ptr == NULL)
    return 0;			/* out of memory! */

  mem->response = ptr;
  memcpy (&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;

  return realsize;
}
