#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_SUB_COMMANDS   5
#define MAX_ARGS           10
struct SubCommand {
	char *line;
	char *argv[MAX_ARGS];
};

struct Command { 
	struct SubCommand sub_commands[MAX_SUB_COMMANDS];
	int num_sub_commands;
	char *stdin_redirect;
	char *stdout_redirect;
	int background;
};



void ReadArgs(char *in, char **argv, int size){
	char *p;
	char *dup;
	//printf("read\n");
	//printf("%s",in);
	dup=strdup(in);
	int i=0;
	p=strtok(dup," ");
	while(p){
		
		argv[i++]=p;
		p=strtok(NULL," ");
		if(i>=size)
			break;
	}
	argv[i]=NULL;
}

void print_args(char **argv){
	int i;
	for(i=0; argv[i]; i++){
		printf("argv[%d]='%s'\n",i,argv[i]);
	}
}

void ReadCommand(char *line, struct Command *command){
	int i;
	char *dup;
	dup=strdup(line);
	dup[strlen(line)-1]='\0';
	command->num_sub_commands=0;
	char *newline=strtok(dup,"|");
	while(newline){
		command->sub_commands[command->num_sub_commands].line=newline;
		//printf("%s\n",dup);
		command->num_sub_commands++;
		newline=strtok(NULL,"|");
	}
	for(i=0;i<command->num_sub_commands;i++){
		ReadArgs(command->sub_commands[i].line,command->sub_commands[i].argv,MAX_ARGS);
	}

}

void readRedirectsAndBackground(struct Command *command){

	int i=command->num_sub_commands;
	int j=0;

	while(command->sub_commands[i-1].argv[j]){
		//printf("%s",command->sub_commands[i-1].argv[j]);
		if (strcmp(command->sub_commands[i-1].argv[j],"<")==0){
			command->stdin_redirect=command->sub_commands[i-1].argv[j+1];
			command->sub_commands[i-1].argv[j++]=NULL;
		}
		else if (strcmp(command->sub_commands[i-1].argv[j],">")==0){
			command->stdout_redirect=command->sub_commands[i-1].argv[j+1];
			command->sub_commands[i-1].argv[j++]=NULL;
		}
		else 
			j++;
		

	} 
	if (command->sub_commands[i-1].argv[j-1]!=NULL && strcmp(command->sub_commands[i-1].argv[j-1],"&")==0){
		command->background=1;
		command->sub_commands[i-1].argv[j-1]=NULL;
	}
	else
		command->background=0;
}

void PrintCommand(struct Command *command){
	int i;
	for(i=0;i<command->num_sub_commands;i++){
		printf("command NO.%d\n",i);
		print_args(command->sub_commands[i].argv);
		printf("\n");
	}
	printf("Redirect stdin: %s\n", command->stdin_redirect);
	printf("Redirect stdout: %s\n", command->stdout_redirect);
	if (command->background == 1) 
		printf("Background: yes\n");
	else 
		puts("Background: no");
}

int ExecuteSubcommand(struct Command *command, int i){
	//printf("test1");
	execvp(command->sub_commands[i].argv[0], command->sub_commands[i].argv);
	fprintf(stderr,"%s: Command not found\n", command->sub_commands[i].argv[0]);
	exit(1);
}

int BuiltinCommand(struct Command *command){
	char *cd_path=NULL;
	if (strcmp(command->sub_commands[0].argv[0],"cd")==0){
		if (chdir(command->sub_commands[0].argv[1])!=0)
			printf("-bash: cd: %s: No such file or dictionary\n", command->sub_commands[0].argv[1]);
	}
	else if(strcmp(command->sub_commands[0].argv[0], "exit")==0){
		printf("logout...\n[Process completed]\n");
		exit(0);
	}
	else if(strcmp(command->sub_commands[0].argv[0], "about")==0)
		printf("shell version: v1.5\nauthor: Xianlong Zhang\ndate:3/2/2016\n");
	return 0;
}

void ExecuteShell(){
	struct passwd *pwd;
    char pathname[1024];
	int history_num;
	char *history[1024];
	int background=0;
	int h;

	while(1){
		char s[1024];
		getcwd(pathname,1024);
		struct Command command;
		memset(&command, '\0', sizeof(struct Command));
		//printf("%s",command.sub_commands[0].argv[0]);
		printf("%s",pathname);
		printf("$ ");
		fgets(s, sizeof s, stdin);
		if (s[0]!='\n')
			history[history_num++]=strdup(s);
		//printf("test2");
		//ReadCommand(s, &command);
		ReadCommand(s, &command);
	//readRedirectsAndBackground(&command);
	//PrintCommand(&command);
		int i=0;
		int sub_num=command.num_sub_commands;
		int ret[sub_num];
		int strnum;
		int equalnum=0;
		char varname[512];
		char varvalue[1024];
		int argvnum=0;
		int varlen=0;

		if (command.sub_commands[0].argv[0]==NULL)
			continue;
		if (strcmp(command.sub_commands[0].argv[0], "history")==0){
			for (h=0;h<history_num-1;h++)
				printf("%8d             %s",h,history[h]);
			    
			continue;
		}
		if (strcmp(command.sub_commands[0].argv[0], "about")==0||strcmp(command.sub_commands[0].argv[0], "cd")==0||strcmp(command.sub_commands[0].argv[0], "exit")==0){
			BuiltinCommand(&command);
			continue;
		}
		
		readRedirectsAndBackground(&command);

		if (sub_num==1){
			//printf("test");
			int ret_1=fork();
			if (ret<0){
				perror("fork failed\n");
				exit(-1);
			}
			else if(ret_1==0){
				if (command.stdin_redirect!=NULL){
					int fd_in=open(command.stdin_redirect, O_RDONLY);
					dup2(fd_in,0);
					close(fd_in);
					if (fd_in<0){
						fprintf(stderr, "%s: File not found\n", command.stdin_redirect);
						exit(1);
					}
				}

				if (command.stdout_redirect!=NULL){
					int fd_out=open(command.stdout_redirect, O_WRONLY|O_CREAT|O_TRUNC, 0660);
					dup2(fd_out,1);
					close(fd_out);
					if (fd_out<0){
						fprintf((stderr), "%s: Cannot create file\n", command.stdout_redirect);
						exit(1);
					}
				}

//-------set enviroment variable

				for (strnum=0; strnum<strlen(command.sub_commands[0].argv[0]);strnum++){
					if (command.sub_commands[0].argv[0][strnum]=='='){
						equalnum=strnum;
						//printf("%d",strnum);
						break;
					}
				}
				
				if (command.sub_commands[0].argv[0][equalnum]=='='){

					for(strnum=0;strnum<equalnum;strnum++){
						varname[strnum]=command.sub_commands[0].argv[0][strnum];
					}
                    //printf("test\n");
					for(strnum=equalnum+1;strnum<strlen(command.sub_commands[0].argv[0]);strnum++){
						varvalue[varlen]=command.sub_commands[0].argv[0][strnum];
						varlen++;
					}
					varvalue[varlen]='\0';

					setenv(varname,varvalue,1);
					//printf("%s:%s",varname,varvalue);
					while(command.sub_commands[0].argv[argvnum+1]!=NULL){
						//printf("test2\n");
						strcpy(command.sub_commands[0].argv[argvnum], command.sub_commands[0].argv[argvnum+1]);
						argvnum++;
					}
					command.sub_commands[0].argv[argvnum]=NULL;
				}				
				//printf("test4");
				ExecuteSubcommand(&command,i);
				
			}
			else{
				if (command.background==0)
					wait(NULL);
				else {
					printf("[%d]\n", ret_1);
					wait(NULL);
					if (getchar()=='\n')
						printf("[%d] finished\n", ret_1);
				}
			}
		}
//-----------------------------------when NO of subcommands>1-------------------------		
		if (sub_num>1){
			int fds[sub_num][2];
			int j;
			for (j=0;j<sub_num-1;j++){
				//pipe(fds[j]);
				if (pipe(fds[j])<0)
					perror("pipe");
			}
			while(1){
				ret[i]=fork();
				if (ret[i]<0){
					perror("fork failed\n");
					exit(-1);
				}
					

				else if (ret[i]==0){

					if (i==0){
						close(fds[i][0]);
						if(command.stdin_redirect!=NULL){
							int fd_in=open(command.stdin_redirect, O_RDONLY,0660);
							if (fd_in<0){
								fprintf(stderr, "%s: File not found\n", command.stdin_redirect);
								break;
							}
							dup2(fd_in,0);
							close(fd_in);
						}
						dup2(fds[i][1],1);
						//close(fds[i][1]);
						//printf("test");
					}

					else if(i==sub_num-1){

						if(command.stdout_redirect!=NULL){
							int fd_out=open(command.stdout_redirect,O_WRONLY|O_CREAT|O_TRUNC, 0660);
							dup2(fd_out,1);
							close(fd_out);
							if (fd_out<0){
								fprintf((stderr), "%s: Cannot create file\n", command.stdout_redirect);
								exit(1);
							}
						}

						for (j=0;j<i-1;j++){
							close(fds[j][0]);
							close(fds[j][1]);
						}
						close(fds[j][1]);
						close(fds[i][0]);
						dup2(fds[j][0],0);
					}
					else{
						for (j=0;j<i-1;j++){
							close(fds[j][0]);
							close(fds[j][1]);
						}
						close(fds[j][1]);
						close(fds[i][0]);
						dup2(fds[j][0],0);	
						dup2(fds[i][1],1);
					}

					ExecuteSubcommand(&command, i);
	
				}

				else{
					//printf("test");
					for (j=0;j<i;j++){
						close(fds[j][0]);
						close(fds[j][1]);
					}
					wait(NULL);
					i++;
					if(i==sub_num){
						if (command.background==1){
							printf("[%d]\n", ret[i-1]);
							//wait(NULL);
							if (getchar()=='\n')
								printf("[%d] finished\n", ret[i-1]);
						}
						break;
					}
				}

			}
		}

	}
}



int main(int argc, char **argv)
{	
	ExecuteShell();
	return 0;
}