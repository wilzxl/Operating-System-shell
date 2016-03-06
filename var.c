# include <stdlib.h>
# include <stdio.h>
int main(){

    printf("Print Variable:\n");
    char *var, *value;
    var = "VAR";
    value = getenv(var);
    if (value){
        printf("[%s]= %s\n", var, value);
    }
    else{
        printf("[%s] No such Variable\n", var);
    }
return 0;
}