#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>

#define INSURANCE 100

static char buffer[2048] = { 0 };
char root[1024] = { 0 };
char s_ip[16] = { 0 };
char s_port[6] = { 0 };

struct async_data
{
  int fd;
  size_t client_status;
  char workspace[2048];
  char error_request[4096];
} client_data;

char *template_greeting = "HTTP/1.1 %d %s\r\n"
  "Server: %s\r\n"
  "Content-Length: %d\r\n"
  "Content-Type: text; charset=UTF-8\r\n\r\n" "%s\r\n";

char *
status (int client_status)
{

  if (client_status == 200)
    return "OK";
  else if (client_status == 400)
    return "Bad Request";
  else if (client_status == 403)
    return "Forbidden";
  else if (client_status == 404)
    return "Not Found";
  else
    return "ERROR";

}

int
setnonblocking (int sock)
{
  int opts;

  opts = fcntl (sock, F_GETFL);
  if (opts < 0)
    {
      perror ("fcntl(F_GETFL)");
      return -1;
    }
  opts = (opts | O_NONBLOCK);
  if (fcntl (sock, F_SETFL, opts) < 0)
    {
      perror ("fcntl(F_SETFL)");
      return -1;
    }

  return 0;
}

void
do_read (struct async_data *ptr)
{
  int rc = recv (ptr->fd, buffer, sizeof (buffer), 0);
  if (rc < 0)
    {
      perror ("read");
      return;
    }
  buffer[rc] = 0;

  if (!memcmp (buffer, "GET", 3))
    {
      ptr->client_status = 200;
      int offset = strlen (root);
      memcpy (ptr->workspace, root, offset);
      size_t i = 0;

      while (i + offset < sizeof (ptr->workspace) - 1)
	{
	  if (isspace (buffer[i + 4]))
	    break;
	  ptr->workspace[offset + i] = buffer[i + 4];
	  i++;
	}
      ptr->workspace[offset + i] = '\0';
    }
  else
    {
      ptr->client_status = 400;
      snprintf (ptr->error_request, sizeof (ptr->error_request), "%s\n",
		"Можно отправлять только GET запросы");
    }

}

void
do_write (struct async_data *ptr)
{

  if (ptr->client_status == 0)
    return;

  struct stat statbuf;
  int ffd;
  void *src = NULL;

  if (ptr->client_status == 200)
    {
      if (access (ptr->workspace, R_OK) < 0)
	{
	  ptr->client_status = 403;
	  snprintf (ptr->error_request, sizeof (ptr->error_request),
		    "%s\n",
		    "Файл недоступен для чтения");
	}
      else if ((ffd = open (ptr->workspace, O_RDONLY)) < 0)
	{
	  ptr->client_status = 404;
	  snprintf (ptr->error_request, sizeof (ptr->error_request),
		    "%s\n", "Ошибка вызова open");
	}
      else if (fstat (ffd, &statbuf) < 0)
	{
	  ptr->client_status = 404;
	  snprintf (ptr->error_request, sizeof (ptr->error_request),
		    "Ошибка вызова функции fstat:%s у файла %s\n",
		    strerror (errno), ptr->workspace);
	}
      else if ((src =
		mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, ffd,
		      0)) == MAP_FAILED)
	{
	  ptr->client_status = 404;
	  snprintf (ptr->error_request, sizeof (ptr->error_request),
		    "%s\n",
		    "Ошибка вызова функции mmap для входного файла");
	}
    }

  size_t greeting_size = (ptr->client_status ==
			  200 ? (size_t) statbuf.
			  st_size : strlen (ptr->error_request)) +
    strlen (template_greeting) + INSURANCE;

  char *greeting = (char *) malloc (greeting_size);

  snprintf (greeting, greeting_size, template_greeting, ptr->client_status,
	    status (ptr->client_status), s_ip, statbuf.st_size,
	    ptr->client_status == 200 ? src : ptr->error_request);

  int rc = send (ptr->fd, greeting, strlen (greeting), 0);

  if (src != MAP_FAILED)
    munmap (src, statbuf.st_size);

  free (greeting);

  if (rc < 0)
    {
      perror ("write");
      return;
    }
}

void
process_error (int fd)
{
  printf ("fd %d error!\n", fd);
}

#define MAX_EPOLL_EVENTS 128
static struct epoll_event events[MAX_EPOLL_EVENTS];
#define BACKLOG 128
int
main (int argc, char **argv)
{
  if (argc < 3)
    {
      printf ("Usage: %s <workspace> <ip>:<port>\n", argv[0]);
      return EXIT_FAILURE;
    }

  if (sizeof (root) - 1 < strlen (argv[1]))
    {
      printf
	("Длина каталога поиска не должна превышать %ld\n",
	 sizeof (root) - 1);
      return EXIT_FAILURE;
    }

  if (sizeof (s_ip) + sizeof (s_port) - 2 <= strlen (argv[2]) - 1)
    {
      if (argv[2][0] == ':')
	{
	  puts ("Правильный формат ip:port");
	  return EXIT_FAILURE;
	}
      else
	{
	  printf
	    ("Длина ip:port не должна превышать %ld\n",
	     sizeof (s_ip) + sizeof (s_port) - 2);
	  return EXIT_FAILURE;
	}
    }

  strcpy (root, argv[1]);

  char *temp;
  int i = 0, j = 0;
  for (i = 0, j = 0, temp = s_ip; argv[2][i] != '\0'; i++)
    {
      if (argv[2][i] == ':')
	{
	  temp = s_port;
	  j = 0;
	  continue;
	}
      temp[j] = argv[2][i];
      j++;
    }

  signal (SIGPIPE, SIG_IGN);
  int efd = epoll_create (MAX_EPOLL_EVENTS);
  int listenfd = socket (AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0)
    {
      perror ("socket");
      return EXIT_FAILURE;
    }

  setnonblocking (listenfd);
  struct sockaddr_in servaddr = { 0 };
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr (s_ip);
  servaddr.sin_port = htons (atoi (s_port));
  if (bind (listenfd, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0)
    {
      perror ("bind");
      return EXIT_FAILURE;
    }

  if (listen (listenfd, BACKLOG) < 0)
    {
      perror ("listen");
      return EXIT_FAILURE;
    }

  struct async_data *listen_ptr =
    (struct async_data *) malloc (sizeof (struct async_data));
  listen_ptr->fd = listenfd;

  struct epoll_event listenev;
  listenev.events = EPOLLIN | EPOLLET;
  listenev.data.ptr = listen_ptr;
  if (epoll_ctl (efd, EPOLL_CTL_ADD, listenfd, &listenev) < 0)
    {
      perror ("epoll_ctl");
      return EXIT_FAILURE;
    }

  struct epoll_event connev;
  int events_count = 1;
  for (;;)
    {
      int nfds = epoll_wait (efd, events, MAX_EPOLL_EVENTS, -1);
      for (int i = 0; i < nfds; i++)
	{
	  if (((struct async_data *) events[i].data.ptr)->fd == listenfd)
	    {
	      int connfd = accept (listenfd, NULL, NULL);
	      if (connfd < 0)
		{
		  perror ("accept");
		  continue;
		}

	      if (events_count == MAX_EPOLL_EVENTS - 1)
		{
		  printf ("Event array is full\n");
		  close (connfd);
		  continue;
		}
	      struct async_data *conn_ptr =
		(struct async_data *) malloc (sizeof (struct async_data));
	      conn_ptr->fd = connfd;
	      conn_ptr->client_status = 0;

	      setnonblocking (connfd);
	      connev.data.ptr = conn_ptr;
	      connev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
	      if (epoll_ctl (efd, EPOLL_CTL_ADD, connfd, &connev) < 0)
		{
		  perror ("epoll_ctl");
		  free (conn_ptr);
		  close (connfd);
		  continue;
		}

	      events_count++;
	    }
	  else
	    {
	      /*Это костыльное упорядочивание, лучше не смог придумать */

	      if (!(events[i].events & EPOLLIN)
		  && (events[i].events & EPOLLOUT)
		  && ((struct async_data *) events[i].data.ptr)->
		  client_status == 0)
		continue;

	      struct async_data *ptr = events[i].data.ptr;
	      if (events[i].events & EPOLLIN)
		do_read (ptr);
	      if (events[i].events & EPOLLOUT)
		do_write (ptr);
	      if (events[i].events & EPOLLRDHUP)
		process_error (ptr->fd);
	      epoll_ctl (efd, EPOLL_CTL_DEL, ptr->fd, &connev);
	      close (ptr->fd);
	      free (ptr);
	      events_count--;
	    }
	}
    }

  return 0;
}
