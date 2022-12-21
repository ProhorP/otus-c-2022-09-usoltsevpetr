#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>

/*чему-то в заголовочнике не объявлен прототип функции seekdir
но в самой реализации есть
*/
void seekdir(DIR *dirp, long loc);

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

  return EXIT_SUCCESS;

}
