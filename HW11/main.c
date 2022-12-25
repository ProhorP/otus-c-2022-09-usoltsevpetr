#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <pcre.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#define PWD_LENGTH 200
#define SIZE_STR_BUF 8192
#define SIZE_BYTES_BUF 30

char save_pwd[PWD_LENGTH] = { 0 };

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}

void *
thread_print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  return ((void *) EXIT_FAILURE);
}

typedef struct entry *entry_link;

typedef struct entry
{
  char *path;
  entry_link next;
} entry;

typedef struct thread_data
{
  entry_link file_path;
  int byte_count;
  pthread_t tid;
  int num;
  GHashTable *url_counter;
  GHashTable *ref_counter;
} thread_data;

thread_data *thread_data_array = NULL;

typedef struct key_value
{
  gpointer key;
  gpointer value;
} key_value;

typedef struct key_value_struct
{
  int count;
  key_value *array;
} key_value_struct;

#define as_hs(A) ((key_value_struct *) user_data)->A
void
hash_to_array (gpointer key, gpointer value, gpointer user_data)
{

  as_hs (array[as_hs (count)]).key = key;
  as_hs (array[as_hs (count)]).value = value;
  as_hs (count)++;

}

void
union_hash_func (gpointer key, gpointer value, gpointer user_data)
{

  gpointer val = g_hash_table_lookup ((GHashTable *) user_data, key);

  if (val)
    *((unsigned long long int *) val) += *((unsigned long long int *) value);
  else
    g_hash_table_insert ((GHashTable *) user_data, key, value);

}

#define get_val(A) *((unsigned long long int *)((key_value *) A)->value)
int
compare (const void *i, const void *j)
{
  return get_val (j) - get_val (i);
}


key_value_struct *
malloc_key_value_struct (GHashTable * hash_table)
{

  key_value_struct *key_value_struct_buf =
    malloc (sizeof (key_value_struct) +
	    g_hash_table_size (hash_table) * sizeof (key_value));
  key_value_struct_buf->count = 0;
  key_value_struct_buf->array = (key_value *) (key_value_struct_buf + 1);

  return key_value_struct_buf;

}

void
print_top_by_value (GHashTable * hash_table, int count)
{

  /*получаем структуру, содержащую массив КлючЗначение и счетчик количества
     для сортировки hash-таблицы */
  key_value_struct *key_value_struct_buf =
    malloc_key_value_struct (hash_table);

  /*заполняем */
  g_hash_table_foreach (hash_table, hash_to_array, key_value_struct_buf);

  /*сортируем массив */
  qsort (key_value_struct_buf->array, key_value_struct_buf->count,
	 sizeof (key_value), compare);

  for (int i = 0; i < key_value_struct_buf->count && count; i++, count--)
    printf ("%s = %llu\n", (char *) key_value_struct_buf->array[i].key,
	    *((unsigned long long int *) key_value_struct_buf->
	      array[i].value));

  /*удаляем массив */
  free (key_value_struct_buf);

}

entry *
new_entry (char *str)
{

  entry *e = (entry *) malloc (sizeof (entry) + strlen (str) + 1);

  if (e == NULL)
    print_error
      ("%s\n",
       "Не удалось выделить память под элемент таблицы");

  e->path = (char *) (e + 1);
  e->next = NULL;
  strcpy (e->path, str);

  return e;

}

thread_data *
new_thread_data (int count_threads)
{

  /* выделяем +1 структуру для объединения */
  thread_data *thread_data_array =
    (thread_data *) malloc ((count_threads + 1) * sizeof (thread_data));

  /*инициализация данных для потоков */
  int i;
  for (i = 0; i <= count_threads; i++)
    {
      thread_data_array[i].file_path = NULL;
      thread_data_array[i].byte_count = 0;
      thread_data_array[i].num = i;

      if (i < count_threads)
	{
	  thread_data_array[i].url_counter =
	    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	  thread_data_array[i].ref_counter =
	    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	}
      else
	{
/* последним элементом добавляются хэш-таблицы для объединения
без автовызова free для корректного освобождения памяти*/
	  thread_data_array[i].url_counter =
	    g_hash_table_new (g_str_hash, g_str_equal);
	  thread_data_array[i].ref_counter =
	    g_hash_table_new (g_str_hash, g_str_equal);
	}
    }

  return thread_data_array;

}

void
union_hash (thread_data * thread_data_array, int count_threads)
{

  for (int i = 0; i < count_threads; i++)
    {

      thread_data_array[count_threads].byte_count +=
	thread_data_array[i].byte_count;

      g_hash_table_foreach (thread_data_array[i].url_counter,
			    union_hash_func,
			    thread_data_array[count_threads].url_counter);
      g_hash_table_foreach (thread_data_array[i].ref_counter,
			    union_hash_func,
			    thread_data_array[count_threads].ref_counter);
    }

}

void
free_thread_data (thread_data * thread_data_array, int count_threads)
{
/*очистка данных по потокам*/
  entry_link temp, next;
  for (int i = 0; i <= count_threads; i++)
    {
      for (temp = thread_data_array[i].file_path, next = NULL;
	   temp != NULL; temp = next)
	{
	  next = temp->next;
	  free (temp);
	}

      g_hash_table_destroy (thread_data_array[i].url_counter);
      g_hash_table_destroy (thread_data_array[i].ref_counter);
    }

  free (thread_data_array);
}

void
add_file_path_thread_data (thread_data * thread_data_array, int count,
			   int count_threads, char *file_path_name)
{

  int index = count % count_threads;
  entry *file_path_old = thread_data_array[index].file_path;
  thread_data_array[index].file_path = new_entry (file_path_name);
  thread_data_array[index].file_path->next = file_path_old;

}

/* /^[^\"]+[^\s]+\s([^\s]+)[^\"]+\"\s[\d]+\s([\d]+)\s\"([^\"]+).*$/gm */
/* 127.0.0.1 - frank [10/Oct/2000:13:55:36 -0700] "GET /apache_pb.gif HTTP/1.0" 200 2326 "http://www.example.com/start.html" "Mozilla/4.08 [en] (Win98; I ;Nav)" */
void *
parse_logs (void *arg)
{

/* переменные вынес наверх, что инициализация была 1 раз */
  int num_t = *((int *) arg);
  int fd, i, j, options = 0, length, erroffset;
  char url[SIZE_STR_BUF] = { 0 };
  char referer[SIZE_STR_BUF] = { 0 };
  char bytes_string[SIZE_BYTES_BUF] = { 0 };
  unsigned long int bytes = 0;
  gpointer val = NULL;
  unsigned long long int *malloc_bytes = NULL;
  char *str, *end_str, *path;
  void *src;
  struct stat statbuf;
  int ovector[30];
  pcre *re;
  const char *error;
  printf ("Номер потока %d\n", num_t);

  char *pattern =
    "^[^\"]+[^\\s]+\\s([^\\s]+)[^\"]+\"\\s[\\d]+\\s([\\d]+)\\s\"([^\"]+).*$";

  //const unsigned char *tables = NULL;
  //setlocale (LC_CTYPE, (const char *) "ru.");
  //tables = pcre_maketables();
  re = pcre_compile (pattern, options, &error, &erroffset, NULL);

  if (!re)
    return thread_print_error ("Failed pcre_compile in thread %d\n", num_t);

  entry_link file_path = thread_data_array[num_t].file_path;

  for (path = NULL; file_path; file_path = file_path->next)
    {

      path = file_path->path;

      if ((fd = open (path, O_RDONLY)) < 0)
	return thread_print_error
	  ("невозможно открыть %s для чтения",
	   path);

      errno = 0;

      if (fstat (fd, &statbuf) < 0)
	return thread_print_error
	  ("Ошибка вызова функции fstat:%s у файла %s\n",
	   strerror (errno), path);

      if ((src =
	   mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fd,
		 0)) == MAP_FAILED)
	return thread_print_error ("%s\n",
				   "Ошибка вызова функции mmap для входного файла");

      str = src;
      end_str = str + statbuf.st_size;

      for (length = 0; str < end_str; str += (length + 1), length = 0)
	{

	  while (str + length < end_str && *(str + length) != '\n')
	    length++;

	  int count = pcre_exec (re, NULL, str, length, 0, 0, ovector, 30);

	  if (count == -1)
	    continue;

	  for (i = 0, j = ovector[2]; j < ovector[3]; i++, j++)
	    url[i] = *(str + j);
	  url[i] = '\0';

	  assert (i <= SIZE_STR_BUF);

	  for (i = 0, j = ovector[4]; j < ovector[5]; i++, j++)
	    bytes_string[i] = *(str + j);
	  bytes_string[i] = '\0';

	  assert (i <= SIZE_BYTES_BUF);

	  for (i = 0, j = ovector[6]; j < ovector[7]; i++, j++)
	    referer[i] = *(str + j);
	  referer[i] = '\0';

	  assert (i <= SIZE_STR_BUF);

	  bytes = atoi (bytes_string);

	  /* byte_count */
	  thread_data_array[num_t].byte_count += bytes;

	  /* url_counter */
	  val =
	    g_hash_table_lookup (thread_data_array[num_t].url_counter, url);

	  if (val)
	    *((unsigned long long int *) val) += bytes;
	  else
	    {
	      malloc_bytes = malloc (sizeof (unsigned long long int));
	      *malloc_bytes = bytes;

	      g_hash_table_insert (thread_data_array[num_t].url_counter,
				   g_strdup (url), malloc_bytes);
	    }

	  /* ref_counter */
	  val =
	    g_hash_table_lookup (thread_data_array[num_t].ref_counter,
				 referer);

	  if (val)
	    (*((unsigned long long int *) val))++;
	  else
	    {
	      malloc_bytes = malloc (sizeof (unsigned long long int));
	      *malloc_bytes = 1;

	      g_hash_table_insert (thread_data_array[num_t].ref_counter,
				   g_strdup (referer), malloc_bytes);
	    }
	}

      munmap (src, statbuf.st_size);

      if (close (fd) < 0)
	return thread_print_error
	  ("Ошибка вызова close(чтение) для файла %s\n",
	   path);

    }

  pcre_free (re);

  return ((void *) EXIT_SUCCESS);

}

int
main (int argc, char **argv)
{

  if (argc != 3)
    print_error ("%s\n",
		 "Неверно введены параметры, должно"
		 "быть так: ./treads path_log_dir count_threads");
  /* сохраним текущий каталог для возврата */
  if (getcwd (save_pwd, PWD_LENGTH) == NULL)
    print_error ("ошибка вызова getcwd");

  int count_threads = atoi (argv[2]);
  if (!count_threads)
    count_threads = 1;
  DIR *dp;
  struct dirent *dirp;

  thread_data_array = new_thread_data (count_threads);

  if ((dp = opendir (argv[1])) == NULL)	/* каталог недоступен */
    print_error
      ("невозможно открыть %s для чтения", argv[1]);

  /*распределение файлов по потокам */
  int i = 0;
  while ((dirp = readdir (dp)) != NULL)
    {
      if (strcmp (dirp->d_name, ".") == 0 || strcmp (dirp->d_name, "..") == 0)
	continue;		/* пропустить каталоги "." и ".." */

      add_file_path_thread_data (thread_data_array, i++, count_threads,
				 dirp->d_name);
    }


  if (closedir (dp) < 0)
    print_error ("невозможно закрыть каталог %s",
		 argv[1]);

  /*work threads */
  /* сменим каталог на каталог с логами, чтобы потоки нашли свои файлы */
  if (chdir (argv[1]) < 0)
    print_error
      ("ошибка вызова функцииsave_pwd для каталога %s\n",
       argv[1]);

  int err;

  for (i = 0; i < count_threads; i++)
    {
      err =
	pthread_create (&thread_data_array[i].tid, NULL, parse_logs,
			(void *) &thread_data_array[i].num);
      if (err != 0)
	print_error ("невозможно создать поток %d\n",
		     i);
    }

  void *tret;

  for (i = 0; i < count_threads; i++)
    {
      err = pthread_join (thread_data_array[i].tid, &tret);
      if (err != 0)
	print_error
	  ("невозможно присоединить поток %d", i);
      printf ("код выхода потока %d: %ld\n", i, (long) tret);
    }

  /* вернем каталог в прежнее положение */
  if (chdir (save_pwd) < 0)
    print_error
      ("ошибка вызова функции chdir для каталога %s\n",
       save_pwd);

  /* объединяем все данные в последнуюю структуру
     выводим данные и очищаем все */
  union_hash (thread_data_array, count_threads);

  printf
    ("общее количество отданных байт = %d\n",
     thread_data_array[count_threads].byte_count);
  printf
    ("10 самых “тяжёлых” по суммарному трафику URL’ов:\n");
  print_top_by_value (thread_data_array[count_threads].url_counter, 10);

  printf
    ("10 наиболее часто встречающихся Referer’ов:\n");
  print_top_by_value (thread_data_array[count_threads].ref_counter, 10);

  free_thread_data (thread_data_array, count_threads);

  return EXIT_SUCCESS;

}
