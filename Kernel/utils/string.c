#include <my_string.h>


int my_strcmp(char *X, char *Y)
{
    while (*X)
    {
        // if characters differ, or end of the second string is reached
        if (*X != *Y) {
            break;
        }
 
        // move to the next pair of characters
        X++;
        Y++;
    }
 
    // return the ASCII difference after converting `char*` to `unsigned char*`
    return *(const unsigned char*)X - *(const unsigned char*)Y;
}

int my_strcpy(char * to, char *from){
    int i;
    for(i =0; from[i] != 0; i++){
        to[i] = from[i];
    }
    to[i] = 0;
    return i;
}

int my_strlen(char * string){
    int i = 0;
    while(string[i]!=0){
        i++;
    }
    return i;
}