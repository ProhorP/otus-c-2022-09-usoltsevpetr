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

#define NAME "echo.socket"
#define FILE_NAME "main.c"
#define BUFFSIZE 1024

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
main ()
{

  int sock, msgsock;
  struct sockaddr_un server;
  char buf[BUFFSIZE];
  struct stat statbuf;

  sock = socket (AF_UNIX, SOCK_STREAM, 0);

  if (sock < 0)
    print_error ("%s\n", "opening stream socket");

  server.sun_family = AF_UNIX;
  strcpy (server.sun_path, NAME);

  if (bind (sock, (struct sockaddr *) &server, sizeof (struct sockaddr_un)))
    print_error ("%s\n", "binding stream socket");

  printf ("Socket has name %s\n", server.sun_path);

  listen (sock, 5);
  for (;;)
    {
      msgsock = accept (sock, 0, 0);

      if (stat (FILE_NAME, &statbuf) < 0)
	snprintf (buf, BUFFSIZE,
		  "Ошибка вызова функции stat:%s\n",
		  strerror (errno));
      else
	snprintf (buf, BUFFSIZE, "Размер файла в байтах:%ld\n",
		  statbuf.st_size);

      send (msgsock, buf, strlen (buf), 0);

      close (msgsock);
    }
  close (sock);
  unlink (NAME);

  return EXIT_SUCCESS;

}
