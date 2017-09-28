#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include "parse.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define HIS_SIZE 5
#define SIZE 256

int his_cnt = 0;
bool wrap;
char curr_dir[SIZE];
char *history[HIS_SIZE];
char *homedir;
struct passwd *pw;
char prompt[SIZE];

extern char* readline(const char*);

void print_history(long int cnt){
  for(long int i = cnt; i>=1; i--){
    long int addr = (his_cnt - i) % HIS_SIZE;
    addr = addr<0 ? addr + HIS_SIZE : addr;

    printf("%ld %s\n", cnt - i + 1, history[addr]);
  }
}

void set_prompt(){
  sprintf(prompt, "\n%s $> ", curr_dir);
}

void init(){
  printf("\nWelcome to ASP Shell\n");
  pw = getpwuid(getuid());
  homedir = pw->pw_dir;

  chdir(homedir);
  getcwd(curr_dir, SIZE);
  set_prompt();

  his_cnt = 0;
  wrap = false;
}

void execute(char *cmd_line){

  pid_t child_pid;
  char *argv[SIZE];
  int argc, status;

  char *cmd = parse_command(cmd_line, argv, &argc);
  //print_cmd(argc, argv);

  if (strcmp(cmd, "ls") == 0){
    child_pid = fork();

    if(child_pid == 0)
      execvp("/bin/ls", argv);
    else
      waitpid(child_pid, &status, 0);
  }
  else if(strcmp(cmd, "cd") == 0){
    chdir(argv[1]);
    getcwd(curr_dir, SIZE);
    set_prompt();
  }
  else if(strcmp(cmd, "rm") == 0){
    child_pid = fork();

    if(child_pid == 0)
      execvp("/bin/rm", argv);
    else
      waitpid(child_pid, &status, 0);
  }
  else if(strcmp(cmd, "history") == 0){
    if(argv[1] != NULL){
      long int n = strtol(argv[1], NULL, 10);

      if(wrap)
        n = n<HIS_SIZE?n:HIS_SIZE;
      else
        n = n<his_cnt?n:his_cnt;

      print_history(n);
    }
    else{
      if(wrap)
        print_history(HIS_SIZE);
      else
        print_history(his_cnt);
    }
  }
  else if(strcmp(cmd, "issue") == 0){
    if(argv[1] != NULL){
      long int n = strtol(argv[1], NULL, 10);

      if(wrap)
        n = n<HIS_SIZE?n:HIS_SIZE-1;
      else
        n = n<his_cnt?n:his_cnt-1;

      long int addr = (his_cnt - n - 1) % HIS_SIZE;
      addr = addr<0 ? addr + HIS_SIZE : addr;

      char *cmd_to_run = (char*) malloc(sizeof(char) * strlen(cmd_line));
      strcpy (cmd_to_run, history[addr]);

      execute(cmd_to_run);
    }
  }
  else if(strcmp(cmd, "exit") == 0){
    exit(0);
  }
  else{
    FILE *fp = NULL;
    fp = fopen(cmd, "rb");
    if(fp != NULL){
      child_pid = fork();

      if(child_pid == 0)
        execvp(cmd, argv);
      else
        waitpid(child_pid, &status, 0);
    }
    else{
      printf("\n%s: command not found", cmd);
    }
  }
  free(cmd_line);
}

int main(){

  init();

  while(1){
    char *cmd_line;

    cmd_line = readline(prompt);
    char *save_in_his = (char*) malloc(sizeof(char) * strlen(cmd_line));
    strcpy (save_in_his, cmd_line);
    
    history[his_cnt] = save_in_his;
    his_cnt = (his_cnt + 1) % HIS_SIZE;
    
    if(his_cnt == 0)
      wrap = true;
    
    execute(cmd_line);
  }
  return 0;
}
