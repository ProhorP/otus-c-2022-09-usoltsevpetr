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
#include <signal.h>
#include <syslog.h>
#include <sys/resource.h>
#include <fcntl.h>

#define BUFFSIZE 1024
#define NAMESIZE 200
#define LOCKFILE "/var/run/daemon_stat.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

char file_name[NAMESIZE] = { 0 };
char socket_name[NAMESIZE] = { 0 };
char ini_file[NAMESIZE] = { 0 };

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

  if (!g_key_file_load_from_file
      (key_file, ini_file, G_KEY_FILE_NONE, &error))
    {
      if (!g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
	g_warning ("Error loading key file: %s", error->message);
      return;
    }

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

static void
sig_hup ()
{
  syslog (LOG_INFO,
	  "Чтение конфигурационного файла");

  read_conf ();
}

void
daemonize (const char *cmd)
{

  int i, fd0, fd1, fd2;
  pid_t pid;
  struct rlimit rl;
  struct sigaction sa;

  openlog (cmd, LOG_CONS, LOG_DAEMON);

  umask (0);

  if (getrlimit (RLIMIT_NOFILE, &rl) < 0)
    perror
      ("невозможно получить максимальный номер дескриптора");

  if ((pid = fork ()) < 0)
    perror ("ошибка вызова функции fork");
  else if (pid != 0)
    exit (EXIT_SUCCESS);
  setsid ();

  sa.sa_handler = SIG_IGN;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction (SIGHUP, &sa, NULL) < 0)
    syslog (LOG_CRIT,
	    "невозможно игнорировать сигнал SIGHUP");

  if ((pid = fork ()) < 0)
    syslog (LOG_CRIT,
	    "ошибка второго вызова функции fork");
  else if (pid != 0)
    exit (EXIT_SUCCESS);

  if (chdir ("/") < 0)
    syslog (LOG_CRIT,
	    "невозможно сделать текущим рабочим каталогом /");

  if (rl.rlim_max == RLIM_INFINITY)
    rl.rlim_max = 1024;
  for (i = 0; i < (int) rl.rlim_max; i++)
    close (i);

  fd0 = open ("/dev/null", O_RDWR);
  fd1 = dup (0);
  fd2 = dup (0);

  if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    syslog (LOG_CRIT,
	    "ошибочные файловые дескрипторы %d %d %d",
	    fd0, fd1, fd2);

}

int
lockfile (int fd)
{
  struct flock fl;
  fl.l_type = F_WRLCK;
  fl.l_start = 0;
  fl.l_whence = SEEK_SET;
  fl.l_len = 0;
  return (fcntl (fd, F_SETLK, &fl));
}

int
already_running (void)
{
  int fd;
  char buf[16];
  fd = open (LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
  if (fd < 0)
    {
      syslog (LOG_ERR, "невозможно открыть %s: %s",
	      LOCKFILE, strerror (errno));
      exit (EXIT_FAILURE);
    }
  if (lockfile (fd) < 0)
    {
      if (errno == EACCES || errno == EAGAIN)
	{
	  close (fd);
	  return 1;
	}
      syslog (LOG_ERR,
	      "невозможно установить блокировку на %s: %s",
	      LOCKFILE, strerror (errno));
      exit (EXIT_FAILURE);
    }
  ftruncate (fd, 0);
  sprintf (buf, "%ld", (long) getpid ());
  write (fd, buf, strlen (buf) + 1);
  return 0;
}

int
main (int argc, char **argv)
{

  int sock, msgsock;
  struct sockaddr_un server;
  char buf[BUFFSIZE];
  struct stat statbuf;
  struct sigaction sa;

  /*Если передается параметр "-test" тогда демон не запускается
     и не создается обработчик сигнала sighub */
  if (argc == 2 && strcmp (argv[1], "-test") == 0)
    {
      strcpy(ini_file, "daemon_stat.ini");
    }
  else
    {
      strcpy(ini_file, "/etc/daemon_stat.ini");

      //запуск демона
      daemonize ("daemon_stat");

      //типовая проверка на дубль демона
      if (already_running ())
	{
	  syslog (LOG_ERR, "демон уже запущен");
	  return EXIT_FAILURE;
	}

      /*устанавливаем блокировку всех сигналов кроме SIGHUB
         с назначением обработчика сигнала 
       */
      sa.sa_handler = sig_hup;
      sigfillset (&sa.sa_mask);
      sigdelset (&sa.sa_mask, SIGHUP);
      sa.sa_flags = 0;
      if (sigaction (SIGHUP, &sa, NULL) < 0)
	{
	  syslog (LOG_ERR,
		  "невозможно перехватить сигнал SIGHUP: %s",
		  strerror (errno));
	  return EXIT_FAILURE;
	}

    }

  /*Далее функционаяльная часть программы */
  read_conf ();

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
