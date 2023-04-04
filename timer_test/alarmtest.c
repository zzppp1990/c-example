#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handler()
{
  printf("hello\n");
}

int main()
{
  int i;
  signal(SIGALRM, handler);
  alarm(2);
  for(i = 1; i < 5; i++)
  {
    sleep(1);
    printf("sleep %d ...\n", i);
  }

  return 0;
}