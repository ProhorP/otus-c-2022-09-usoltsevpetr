#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <glib.h>
#include <glib/gprintf.h>

/*чему-то в заголовочнике не объявлен прототип функции seekdir
но в самой реализации есть
*/
void seekdir (DIR * dirp, long loc);

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}

int
main (int argc, char **argv)
{

  if (argc != 2)
    print_error ("%s\n",
		 "Укажите путь к каталогу с логами");

  DIR *dp;
  struct dirent *dirp;
  int len_path_buf = 0;

  if ((dp = opendir (argv[1])) == NULL)	/* каталог недоступен */
    print_error ("невозможно открыть %s для чтения",
		 argv[1]);

  while ((dirp = readdir (dp)) != NULL)
    {
      if (strcmp (dirp->d_name, ".") == 0 || strcmp (dirp->d_name, "..") == 0)
	continue;		/* пропустить каталоги "." и ".." */
      len_path_buf += strlen (dirp->d_name) + 1;

    }

  char *path_buf = (char *) malloc (len_path_buf * sizeof (char));
  char *path_buf_offset = path_buf;

  seekdir (dp, 0);

  while ((dirp = readdir (dp)) != NULL)
    {
      if (strcmp (dirp->d_name, ".") == 0 || strcmp (dirp->d_name, "..") == 0)
	continue;		/* пропустить каталоги "." и ".." */
      strcpy (path_buf_offset, dirp->d_name);

      path_buf_offset += strlen (dirp->d_name) + 1;

    }

  free (path_buf);

  if (closedir (dp) < 0)
    print_error ("невозможно закрыть каталог %s",
		 argv[1]);

  GHashTable *hash_table =
    g_hash_table_new ((GHashFunc) NULL, g_str_equal);

  char *key = "aaa";
  gchar *key1 = g_strdup (key);
  gchar *key2 = g_strdup (key);

  gboolean match = g_str_equal ((gconstpointer) key1,
				(gconstpointer) key2);

  int *val_malloc = malloc (sizeof (int));
  *val_malloc = 1;

  g_hash_table_insert (hash_table, (gconstpointer) key1, (gpointer) val_malloc);

  gpointer val = g_hash_table_lookup (hash_table, (gconstpointer) key2);

  if (val)
    *((int *) val) += 1;
  else
    g_hash_table_insert (hash_table, (gpointer) g_strdup (key),
			 (gpointer) val_malloc);

  val = g_hash_table_lookup (hash_table, (gconstpointer) key);

  g_hash_table_destroy (hash_table);


  return EXIT_SUCCESS;

}
