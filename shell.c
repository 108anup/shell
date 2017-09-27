#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

char curr_dir[256];

void print_prompt(){
  printf("\n%s $> ", curr_dir);
}

void init(){
  strcpy(curr_dir, "~");
}

int main(int argc, char **argv){

  init();

  while (1){
		int child_pid;
		char *cmd_line;

    print_prompt();

    cmd_line = readline("");
    
		cmd = parse_command(cmd_line);

    //TODO::
		//record command in history list (GNU readline history ?)

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
}
