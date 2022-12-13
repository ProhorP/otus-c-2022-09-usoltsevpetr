#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib/gprintf.h>

#define BUFFSIZE 1024
#define NAMESIZE 200 

char file_name[NAMESIZE] = { 0 };
char socket_name[NAMESIZE] = { 0 };

void
print_error (const char *format, ...)
{
  va_list argptr;
  va_start (argptr, format);
  vprintf (format, argptr);
  va_end (argptr);
  exit (EXIT_FAILURE);
}

void
read_conf (void)
{

  g_autoptr (GError) error = NULL;
  g_autoptr (GKeyFile) key_file = g_key_file_new ();

  if (!g_key_file_load_from_file (key_file, "daemon_stat.ini", G_KEY_FILE_NONE, &error))
    {
      if (!g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
	g_warning ("Error loading key file: %s", error->message);
      return;
    }

  //g_key_file_free (key_file);
return;

  g_autofree gchar *g_file_name =
    g_key_file_get_string (key_file, "First Group", "FileName", &error);
  if (g_file_name == NULL
      && !g_error_matches (error, G_KEY_FILE_ERROR,
			   G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    {
      g_warning ("Error finding key in key file: %s", error->message);
      return;
    }
  else if (g_file_name == NULL)
    {
      // Fall back to a default value.
      g_file_name = g_strdup ("default-value");
    }

  strcpy (file_name, (char *) g_file_name);

  g_autofree gchar *g_socket_name =
    g_key_file_get_string (key_file, "First Group", "SocketName", &error);
  if (g_socket_name == NULL
      && !g_error_matches (error, G_KEY_FILE_ERROR,
			   G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    {
      g_warning ("Error finding key in key file: %s", error->message);
      return;
    }
  else if (g_socket_name == NULL)
    {
      // Fall back to a default value.
      g_socket_name = g_strdup ("default-value");
    }


  strcpy (socket_name, (char *) g_socket_name);


}

int
main ()
{


  int sock, msgsock;
  struct sockaddr_un server;
  char buf[BUFFSIZE];
  struct stat statbuf;

  read_conf ();
return 0;


  sock = socket (AF_UNIX, SOCK_STREAM, 0);

  if (sock < 0)
    print_error ("%s\n", "opening stream socket");

  server.sun_family = AF_UNIX;
  strcpy (server.sun_path, socket_name);

  if (bind (sock, (struct sockaddr *) &server, sizeof (struct sockaddr_un)))
    print_error ("%s\n", "binding stream socket");

  printf ("Socket has name %s\n", server.sun_path);

  listen (sock, 5);
  for (;;)
    {
      msgsock = accept (sock, 0, 0);

      if (stat (file_name, &statbuf) < 0)
	snprintf (buf, BUFFSIZE,
		  "Ошибка вызова функции stat:%s у файла %s\n",
		  strerror (errno), file_name);
      else
	snprintf (buf, BUFFSIZE,
		  "Размер файла %s в байтах:%ld\n",
		  file_name, statbuf.st_size);

      send (msgsock, buf, strlen (buf), 0);

      close (msgsock);
    }
  close (sock);
  unlink (socket_name);

  return EXIT_SUCCESS;

}
