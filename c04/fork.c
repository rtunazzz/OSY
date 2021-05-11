#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main ()
{
  pid_t child_pid;

  printf ("Parent process [before fork()]: PID = %d\n", (int) getpid ());

  /* Duplicate this process */
  child_pid = fork ();

  switch( child_pid ) 
  {
    case -1:  /* Error */
      fprintf(stderr,"Error: fork()");  
      exit(EXIT_FAILURE);

    case 0:   /* Child code */
      printf ("Child process: PID = %d\n", (int) getpid ());
      sleep(60);
      printf ("Child process: end\n"); 
      break;

    default:  /* Parent code */ 
      printf ("Parent process: PID = %d\n", (int) getpid ());
      printf ("                the child's PID = %d\n", (int) child_pid);
      sleep(30);
      printf ("Parent process: end\n");
  }
  return EXIT_SUCCESS;
}
