#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void * allocString(int size);

// first 4 bytes -- a counter 
void printString(void * str){
    char * my_string = (char*)(str);
    printf("%s\n",(my_string+4));
}

void printInt(int value){
    printf("%d\n",value);
}

int readInt(){
    int value;
    scanf("%d",&value);
    char ch;
    while((ch=getchar())!=EOF)
    {
        if (ch == '\n')
            break; 
    }
    
    return value;
}

void increaseStringCounter(void *str){
    (*(int*)(str))++;
}

void decreaseStringCounter(void *str){
    (*(int*)(str))--;
    if(*(int*)(str)==0){
        printf("Deallocating space for string\n");
        free(str);
    }
    else{
        printf("Counter: %d for string => ",(*(int*)(str)));
        printString(str);
    }
}

void * readString(){
    char *str=NULL;
    size_t n;
    size_t len = getline(&str,&n,stdin);

    if(len>0 && str[len-1]=='\n')
        str[len-1]='\0';
    

    void * my_string = allocString(n);

    memcpy((char*)(my_string)+4,str,n+1);

    return my_string;
}

void * addStrings(void *str_1,void *str_2){
    // lengths without counters
    int n_1=strlen((char*)(str_1)+4);
    int n_2=strlen((char*)(str_2)+4);

    void * str = allocString(n_1+n_2);
    memcpy((char*)(str)+4,(char*)(str_1)+4,n_1);
    memcpy((char*)(str)+4+n_1,(char*)(str_2)+4,n_2+1);
    return str;
}

void * allocString(int size){
    void * str = malloc(sizeof(char)*(4+size+1));
    *(int*)(str)=0; // first 4 bytes are set to 0 
    return str;
}

void freeString(void * str){
    free(str);
}


