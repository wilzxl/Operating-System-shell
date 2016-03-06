# Operating-System-shell
Project Name: Operating System-Shell
Author: Xianlong Zhang
Text Applcation: Sublime Text2
Finished on MAC OS

Functions for fun:

Enter "about", you will get the simple information of the shell.
Enter "exit", you will logout from the shell, if you enter into the shell again through itself, exit twice, please.


Required and Optional Functions: 

All requirements except for "up and down arrow" function are fulfilled:
1) When a command is launched in the background using operator , the shell will print immediately the pid of the process corresponding to the last sub-command, and after you press "ENTER", the message that the pid of the process finished, the finish message won't appear unless the process is really finished.

For example:
$ ls -l | wc &
       8      65     440
[3256]
//press ENTER
[3256] finished
$

2) When a sub-command is not found, a proper error message will be displayed, immediately followed by the next prompt.

For example:
$ a
a: Command not found
$

3) The output redirection operator should redirect the standard output of the last sub-command of a command. If the output file cannot be created, a proper message should be displayed. 

For example:
$ ls -l > invalid/path
invalid/path: Cannot create file
$ ls -l > out.txt
$ cat out.txt
< Contents of “out.txt” displayed here >

4) The input redirection operator will redirect the standard input of the first sub-command of a command. If the input file is not found, a proper message should be displayed. 

For example:
$ wc < out.txt
    9     74     505
$ wc < invalid_file
invalid_file: File not found

5) The command prompt displays the current directory, and we can implement the functionality of built-in command cd to change it.

For example:
/Users/XianlongZhang/Desktop/shellproject$ cd ..
/Users/XianlongZhang/Desktop$ cd shell project
/Users/XianlongZhang/Desktop/shellproject$ 

6) Implement support for process-specific environment variables. These variables can be specified with a pair VAR=VALUE appearing as a prefix of the command.  

For example( NOTE: BEFORE do this, please compile the attached var.c by "gcc var.c -o var" FIRST or just use the "var" attached in the folder. The environment variable name must be VAR):
/Users/XianlongZhang/Desktop/shellproject$ VAR=hello ./var
Print Variable:
[VAR]= hello

7) The function history can be implemented, if you just press ENTER, it won't be saved in history.

For example:
/Users/XianlongZhang/Desktop/shellproject$ 
/Users/XianlongZhang/Desktop/shellproject$ ls
/Users/XianlongZhang/Desktop/shellproject$ history
       0             ls
