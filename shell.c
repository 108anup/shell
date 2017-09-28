#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <pwd.h>

#define SIZE 256

char curr_dir[SIZE];
char *history[SIZE];
char *homedir;
struct passwd *pw;
char prompt[SIZE];

void set_prompt(){
  sprintf(prompt, "\n%s $> ", curr_dir);
}

void init(){
  pw = getpwuid(getuid());
  homedir = pw->pw_dir;

  chdir(homedir);
  getcwd(curr_dir, SIZE);
  set_prompt();
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
      wait(child_pid, &status, 0);
  }
  else if(strcmp(cmd, "cd") == 0){
    chdir(argv[1]);
    getcwd(curr_dir, SIZE);
    set_prompt();
  }
  //TODO:: execute :
  /*
    if ( isBuiltInCommand(cmd)){
    executeBuiltInCommand(cmd);
    } else {    
    childPid = fork();
    if (childPid == 0){
    executeCommand(cmd); //calls execvp  
      
    } else {
    if (isBackgroundJob(cmd)){
    record in list of background jobs
    } else {
    waitpid (childPid);

    }   
    }
    }*/
}

int main(){

  init();
  int i = 0;

  while(1){
    char *cmd_line;

    cmd_line = readline(prompt);

    int l = strlen(cmd_line);

    history[i] = cmd_line;
    i = (i + 1) % SIZE;

    execute(cmd_line);
  }
  return 0;
}
