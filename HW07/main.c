#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>

#define INT_FORMAT "%ld "

typedef struct list
{
  long int value;
  struct list *next;
} list;

long int data[] = { 4, 8, 15, 16, 23, 42 };

char *empty_str = "";

void print_int (long int a);
long int p (long int a);
list *add_element (long int value, list * b);
void m (list * a, void func (long int c));
list *f (list * a, list * b, long int func (long int d));
void list_free (list * a);

int
main ()
{

  list *a = NULL;

  long int n = sizeof (data) / sizeof (long int);

  for (; n; a = add_element (data[n - 1], a), n--)
    ;

  m (a, print_int);

  puts (empty_str);

  list *b = f (a, NULL, p);

  m (b, print_int);

  puts (empty_str);

  list_free (a);
  list_free (b);

  return 0;

}

void
print_int (long int a)
{

  printf (INT_FORMAT, a);
  fflush (NULL);

}

long int
p (long int a)
{
  return a & 0x01;
}

list *
add_element (long int value, list * b)
{

  list *c;

  if ((c = (list *) malloc (sizeof (list))) == NULL)
    abort ();

  c->value = value;
  c->next = b;

  return c;
}

void
m (list * a, void func (long int c))
{

  if (!a)
    return;

  func (a->value);
  m (a->next, func);

  return;
}

void
list_free (list * a)
{

  if (a->next)
    list_free (a->next);

  free (a);
}

list *
f (list * a, list * b, long int func (long int d))
{

  if (!a)
    return b;

  list *c;

  if (!func (a->value))
    c = b;
  else
    c = add_element (a->value, b);

  return f (a->next, c, func);

}
