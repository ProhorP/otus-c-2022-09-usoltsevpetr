#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <glib.h>
#include <glib/gprintf.h>

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
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
  GHashTable *url_counter;
  GHashTable *ref_counter;
} thread_data;

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
    *((int *) val) += *((int *) value);
  else
    g_hash_table_insert ((GHashTable *) user_data, key, value);

}

#define get_val(A) *((int *)((key_value *) A)->value)
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
  key_value_struct_buf->array =
    (key_value *) key_value_struct_buf + sizeof (key_value_struct);

  return key_value_struct_buf;

}

void
print_top_by_value (GHashTable * hash_table)
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

  for (int i = 0; i < key_value_struct_buf->count; i++)
    printf ("%s = %d\n", (char *) key_value_struct_buf->array[i].key,
	    *((int *) key_value_struct_buf->array[i].value));

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

  e->path = (char *) e + sizeof (entry);
  e->next = NULL;
  strcpy (e->path, str);

  return e;

}

thread_data *
new_thread_data (int count_threads)
{

  thread_data *thread_data_array =
    (thread_data *) malloc (count_threads * sizeof (thread_data));

  /*инициализация данных для потоков */
  int i;
  for (i = 0; i < count_threads; i++)
    {
      thread_data_array[i].file_path = NULL;
      thread_data_array[i].byte_count = 0;
      thread_data_array[i].url_counter =
	g_hash_table_new (g_str_hash, g_str_equal);
      thread_data_array[i].ref_counter =
	g_hash_table_new (g_str_hash, g_str_equal);
    }

  return thread_data_array;

}

void
union_hash (thread_data * thread_data_array, int count_threads)
{

  for (int i = 1; i < count_threads; i++)
    {

      thread_data_array[0].byte_count += thread_data_array[i].byte_count;

      g_hash_table_foreach (thread_data_array[i].url_counter, union_hash_func,
			    thread_data_array[0].url_counter);
      g_hash_table_foreach (thread_data_array[i].ref_counter, union_hash_func,
			    thread_data_array[0].ref_counter);
    }

}

void
free_thread_data (thread_data * thread_data_array, int count_threads)
{
/*очистка данных по потокам*/
  entry_link temp, next;
  for (int i = 0; i < count_threads; i++)
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

int
main (int argc, char **argv)
{

  if (argc != 3)
    print_error ("%s\n",
		 "Неверно введены параметры, должно"
		 "быть так: ./treads path_log_dir count_threads");

  int count_threads = atoi (argv[2]);
  DIR *dp;
  struct dirent *dirp;
  thread_data *thread_data_array = new_thread_data (count_threads);

  if ((dp = opendir (argv[1])) == NULL)	/* каталог недоступен */
    print_error ("невозможно открыть %s для чтения",
		 argv[1]);

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

/*work threads*/

#if 1
  union_hash (thread_data_array, count_threads);

  printf ("общее количество отданных байт = %d\n",
	  thread_data_array[0].byte_count);
  printf
    ("10 самых “тяжёлых” по суммарному трафику URL’ов:\n");
  print_top_by_value (thread_data_array[0].url_counter);
  printf
    ("10 наиболее часто встречающихся Referer’ов:\n");
  print_top_by_value (thread_data_array[0].ref_counter);

  free_thread_data (thread_data_array, count_threads);

#endif

#if 0
  GHashTable *hash_table1 = g_hash_table_new (g_str_hash, g_str_equal);
  GHashTable *hash_table2 = g_hash_table_new (g_str_hash, g_str_equal);

  char *key1 = "aaa1";
  char *key2 = "aaa2";

  int *val_malloc1 = malloc (sizeof (int));
  *val_malloc1 = 1;
  int *val_malloc2 = malloc (sizeof (int));
  *val_malloc2 = 4;

  g_hash_table_insert (hash_table1, (gpointer) g_strdup (key1),
		       (gpointer) val_malloc1);
  g_hash_table_insert (hash_table1, (gpointer) g_strdup (key2),
		       (gpointer) val_malloc2);

  g_hash_table_insert (hash_table2, (gpointer) g_strdup (key1),
		       (gpointer) val_malloc1);

  g_hash_table_foreach (hash_table1, union_hash_func, hash_table2);

  printf ("Топ самых:\n");
  print_top_by_value (hash_table1);

  gpointer val =
    g_hash_table_lookup (hash_table1, (gconstpointer) g_strdup (key1));

  if (val)
    *((int *) val) += 5;
  else
    g_hash_table_insert (hash_table1, (gpointer) g_strdup (key1),
			 (gpointer) val_malloc1);

  val = g_hash_table_lookup (hash_table1, (gconstpointer) key1);

  g_hash_table_destroy (hash_table1);

#endif

  return EXIT_SUCCESS;

}
