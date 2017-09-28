#include <string.h>

char* parse_command(char *args, char *argv[], int *argc){
  char *token, *save_ptr;
  *argc = 0;
  for (token = strtok_r (args, " ", &save_ptr); token != NULL;
       token = strtok_r (NULL, " ", &save_ptr))
  {
    argv[*argc] = token;
    (*argc)++;
  }
  argv[*argc] = NULL;
  return argv[0];
}

void print_cmd(int argc, char *argv[]){
  int i = 0;
  printf("\nargc: %d", argc);
  for(i; i<argc; i++){
    printf("\nargv[%d]: %s", i, argv[i]);
  }
}
