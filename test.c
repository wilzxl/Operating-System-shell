#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

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

int main(int argc, char **argv)
{	
	char s[1024];
	struct Command com;
	//com=(struct Command*)malloc(sizeof(struct Command*));
	printf("Enter a string:");
	fgets(s, 1024, stdin);
	ReadCommand(s, &com);
	readRedirectsAndBackground(&com);
	PrintCommand(&com);
	return 0;
}
