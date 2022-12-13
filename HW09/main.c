#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define NAME "echo.socket"

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

  int sock, msgsock, rval;
  struct sockaddr_un server;
  char buf[1024];

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
      if (msgsock == -1)
	{
	  perror ("accept");
	  break;
	}
      else
	do
	  {
	    bzero (buf, sizeof (buf));
	    if ((rval = read (msgsock, buf, 1024)) < 0)
	      perror ("reading stream message");
	    else if (rval == 0)
	      printf ("Ending connection\n");
	    else
	      send (msgsock, buf, strlen (buf), 0);
	  }
	while (rval > 0);
      close (msgsock);
    }
  close (sock);
  unlink (NAME);

  return EXIT_SUCCESS;

}
