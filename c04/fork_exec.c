#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main ()
{
  pid_t child_pid;
  int status;

  /* Duplicate this process */
  printf ("Parent process: fork()\n");
  child_pid = fork ();

  switch( child_pid ) 
  {
    case -1:  /* Error */
      fprintf(stderr,"Error: fork()");
      exit(EXIT_FAILURE);

    case 0:   /* Child code */
      printf ("Child process: execlp()\n");
      execlp("sleep", "sleep", "30", (char *) NULL);
      fprintf(stderr,"Error: execlp()");
      exit(EXIT_FAILURE); 

    default:  /* Parent code */
      printf ("Parent process: wait()\n");
      wait(&status);   
      printf ("Parent process: end\n"); 
  }
  return EXIT_SUCCESS;
}
