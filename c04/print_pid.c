#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
  printf ("\nThe process id is %d\n", (int) getpid ());
  printf ("The parent process id is %d\n", (int) getppid ());

  sleep(30);

  return 0;
}
