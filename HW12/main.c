#define _POSIX_C_SOURCE 200112L

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define PORT 23
#define BUF_SIZE 4096
#define HOSTNAME "telehack.com"
#define SERVICE "telnet"
#define QUERY_SIZE 1024

static char buffer[BUF_SIZE] = { 0 };
char query_string[QUERY_SIZE] = { 0 };

int
main (int argc, char **argv)
{

  if (argc != 3)
    {
      printf ("Usage: %s шрифт текст\n", argv[0]);
      return EXIT_FAILURE;
    }

  snprintf (query_string, QUERY_SIZE, "figlet /%s %s\r\n", argv[1], argv[2]);

  struct addrinfo *result, *rp;
  struct addrinfo hints;
  int sfd;

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_INET;	/* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM;	/* Datagram socket */
  hints.ai_flags = 0;		/* For wildcard IP address */
  hints.ai_protocol = IPPROTO_TCP;

  int s = getaddrinfo (HOSTNAME, SERVICE, &hints, &result);
  if (s != 0)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
      exit (EXIT_FAILURE);
    }

  /* getaddrinfo() returns a list of address structures.
     Try each address until we successfully connect(2).
     If socket(2) (or connect(2)) fails, we (close the socket
     and) try the next address. */

  for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1)
	continue;

      if (connect (sfd, rp->ai_addr, rp->ai_addrlen) != -1)
	break;			/* Success */

      close (sfd);
    }

  if (rp == NULL)
    {				/* No address succeeded */
      fprintf (stderr, "Could not connect\n");
      exit (EXIT_FAILURE);
    }

  freeaddrinfo (result);	/* No longer needed */

  int len = 0, r = 0;

  while ((r = recv (sfd, &buffer[len], BUF_SIZE - len, 0)) > 0)
    {
      len += r;
      if (buffer[len - 1] == '.' && buffer[len - 2] == '\n')
	break;
    }

  if (send (sfd, query_string, strlen (query_string), 0) < 0)
    {
      perror ("send");
      close (sfd);
      return EXIT_FAILURE;
    }

  len = 0, r = 0;
  while ((r = recv (sfd, &buffer[len], BUF_SIZE - len, 0)) > 0)
    {
      len += r;
      if (buffer[len - 1] == '.' && buffer[len - 2] == '\n')
	break;
    }


  if (r < 0)
    {
      perror ("recv");
      close (sfd);
      return EXIT_FAILURE;
    }

  buffer[len] = '\0';

  puts (buffer);
  shutdown (sfd, SHUT_RDWR);
  close (sfd);

  return EXIT_SUCCESS;

}
