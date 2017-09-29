#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h>
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
  else if(strcmp(cmd, "rmexcept") == 0){
    
    char str[256];
    sprintf(str,"%s/tmp",homedir);
    mkdir(str,0777);
    int i = 1;
    while(argv[i]!=NULL)
    {
      sprintf(str,"%s/tmp/%s",homedir,argv[i]);
      printf("\n%s %s\n",str,argv[i]);
      rename(argv[i],str);
      i++;
    }

    child_pid = fork();
    
    if(child_pid == 0)
    { 
      execlp("/bin/rm", "rm", "-r",curr_dir,NULL);
      exit(0);
    }
    else
    {
      waitpid(child_pid, &status, 0);
      sprintf(str,"%s/tmp",homedir);
      rename(str,curr_dir);
      chdir(curr_dir);
    }
  }
  else{
    FILE *fp = NULL;
    fp = fopen(cmd, "rb");

    if(fp != NULL){
      fclose(fp);
      child_pid = fork();

      if(child_pid == 0)
      {
        int o_red_index = -1, i_red_index = -1;

        get_redirection(argv, &o_red_index, &i_red_index);
        
        int ofd = -1;
        int ifd = -1;

        if(o_red_index != -1)
        {
          ofd = open(argv[o_red_index] , O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR | S_IWUSR);
          if(ofd < 0)
          {
            printf("Redirection to %s failed",argv[o_red_index]);
            exit(-1);
          }
          dup2(ofd,STDOUT_FILENO);
          close(ofd);
        }

        if(i_red_index != -1)
        {
          ifd = open(argv[i_red_index], O_RDONLY);
          if( ifd < 0 )
          {
            printf("Redirection from %s failed",argv[o_red_index]);
            exit(-1);  
          }
          dup2(ifd,STDIN_FILENO);
          close(ifd);
        }    
        int err = execv(cmd, argv);
        printf("Error occured in execution: %d",err);
        exit(err);
      }
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
    if ( strcmp(cmd_line,"")!=0 )
    {
      char *save_in_his = (char*) malloc(sizeof(char) * strlen(cmd_line));
      strcpy (save_in_his, cmd_line);
      
      history[his_cnt] = save_in_his;
      his_cnt = (his_cnt + 1) % HIS_SIZE;
      
      if(his_cnt == 0)
        wrap = true;
      
      execute(cmd_line);
    }
  }
  return 0;
}