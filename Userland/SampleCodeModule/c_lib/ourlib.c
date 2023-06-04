#include <syscallslib.h>

#define STDIN 1
#define MAXBUFF 100000

void my_substring(char* dest, const char* src, int n, int m) {
    int i, j;

    for (i = n, j = 0; i <= m; i++, j++) {
        dest[j] = src[i];
    }

    dest[j] = '\0';
}

int strlen(char * string){
    int i = 0;
    while(string[i]!=0){
        i++;
    }
    return i;
}

void clear(void){
    sys_clear();
}

void putchar(char c){
    sys_write(STDIN,&c,1);
}

void printf(char * string){
    sys_write(1,string,strlen(string));
}

void printerr(char * string){
    sys_write(2,string,strlen(string));
}

void printFirst(char * string){
    sys_write(3,string,strlen(string));
}

int scanf(char * buffer){
    return sys_read(STDIN,buffer,MAXBUFF);
}

int strcmp(char*str1,char*str2){ //devuelve 0 si son distintos y 1 si son iguales
    int pos = 0;
    while(str1[pos]&&str2[pos]){
        if(str1[pos]!=str2[pos])
            return 0;
        pos++;
    }
    if(str1[pos]||str2[pos])
        return 0;
    return 1;
}

int strcpy(char * to, char *from){
    int i;
    for(i =0; from[i] != 0; i++){
        to[i] = from[i];
    }
    to[i] = 0;
    return i;
}

int atoi(char * str){
    int res = 0;
    for(int i = 0; str[i] != '\0';i++){
        res = res*10+str[i]-'0';
    }
    return res;
}

void reverseString(char* str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void intToString(int num, char* str) {
    int i = 0;
    int isNegative = 0;

    // Handle the case of a negative number
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    // Convert individual digits to characters
    while (num != 0) {
        int remainder = num % 10;
        str[i++] = '0' + remainder;
        num = num / 10;
    }

    // Add the negative sign if necessary
    if (isNegative) {
        str[i++] = '-';
    }

    // Add null terminator
    str[i] = '\0';

    // Reverse the string
    reverseString(str, i);
}

uint32_t cUintToBase(uint64_t value, char * buffer, uint32_t base)
{
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    //Calculate characters for each digit
    do
    {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    }
    while (value /= base);

    // Terminate string in buffer.
    *p = 0;

    //Reverse string in buffer.
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

uint64_t stringToUint64(char *hex, int *ok) {
    uint64_t val = 0;
    int len = 0;
    while (*hex && len <8 ) {
        // get current character then increment
        uint8_t byte = *hex++;
	
        if (!(byte >= '0' && byte <= '9') && !(byte >= 'a' && byte <= 'f') && !(byte >= 'A' && byte <= 'F')) {
          *ok = 0;
          return 0;
        }
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
	len++;
    }
    return val;
}