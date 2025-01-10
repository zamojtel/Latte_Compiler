#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

extern "C"{

int string_count = 0;
struct A{
    int a;
    bool b;
    bool c;
    const char * str;
};

class Dice{
public:
    int m_faces;
    int roll(int x){
        return (this->m_faces+1)%6;
    }
};

class B{
    int x;
    int y;
    bool z;
    int *t;
};

// Arrays
// tablica na dane, a na jej poczatku licznik/rozmiar
// rozmiar jako int 4 bajtowy 
int getArrayLength(void *arr){
    int *size = (int*)(arr);
    // krok wstecz do rozmiaru
    size--;
    return *size;
}

void * allocArray(int item_count,int item_size){
    // sizeof(int) for storing size of the array
    int *arr = (int*)malloc(item_size*(item_count)+sizeof(int));
    arr[0] = item_count;
    // zerowanie elementow tablicy
    memset(arr+1,0,item_count*item_size);
    // 0 0 0 0 0 0 0 0 kazdy bajt
    // +1 zwracamy adress tablicy
    return arr+1;
}

int getStringCount(){
    return string_count;
}

void * allocString(int size);
// idea na poczatku znajduje sie flaga mowiaca czy mozemy modyfikowac str:
// bo zmienne @.str sa niemodyfikowalne naszymi str zaalokowanymi dyn. mozemy z nimi robic co chcemy
// Pierszy bajt 0 -> ozn. string niemodyfikowalny
// Pierszy bajt 1 -> ozn. string modyfikowalny
// Nastepne 4 bajty to counter
// reszta str literal

// first 4 bytes -- a counter 
void printString(void * str){
    if(str==nullptr)
    {
        printf("\n");
        return;
    }

    char * my_string = (char*)(str);
    // printf("%s\n",(my_string+4));
    printf("%s\n",(my_string+5));
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
    // przesuniecie o 1 bajt, o licznik
    char * p = (char*)(str);
    if(p[0]) // sprawdzenie flagi
        (*(int*)(p+1))++;
}

void decreaseStringCounter(void *str){
    // przesuniecie o 1 bajt, o licznik
    // (char*)(str)++;
    char * p = (char*)(str);
    if(p[0]){
        (*(int*)(p+1))--;
        if(*(int*)(p+1)==0){
            string_count--;
            free(str);
        }
    }
}

void * readString(){
    char *str=NULL;
    size_t n;
    size_t len = getline(&str,&n,stdin);

    if(len>0 && str[len-1]=='\n')
        str[len-1]='\0';

    void * my_string = allocString(n);
    // 4 na counter 1 na flage bool czy modifikowalny
    memcpy((char*)(my_string)+4+1,str,n+1);
    return my_string;
}

// string jest ustawiany na null
// 2 stringi sa rowne jak nie sa nullami i sa rowne albo jak oba sa nullami 
// w printString i konkatencacji string null jest tak samo trakowany jak pusty string 
void * addStrings(void *str_1,void *str_2){
    // lengths without counters
    int n_1=(str_1 ? strlen((char*)(str_1)+4+1) : 0);
    int n_2=(str_2 ? strlen((char*)(str_2)+4+1) : 0);

    uint8_t * str = (uint8_t *)allocString(n_1+n_2);
    
    if(n_1)
        memcpy(str+4+1,(char*)(str_1)+4+1,n_1);
    if(n_2)
        memcpy(str+4+1+n_1,(char*)(str_2)+4+1,n_2);
    
    *(str+n_1+n_2+4+1)=0;

    return str;
}
// compare_strings
bool compareStrings(int8_t *str_1,int8_t *str_2){
    if(str_1==str_2)
        return true;
    if(str_1==nullptr || str_2==nullptr)
        return false;

    return strcmp((char *)(str_1+4+1),(char *)(str_2+4+1));
}

void * allocString(int size){
    // 1 -> \0 , 4-> counter, 1 -> flaga
    char * str = (char*)malloc(sizeof(char)*(4+size+1+1));
    // ustawienie flagi
    str[0]=1;
    // ustawienie licznika
    *(int*)(str+1)=0; // first 4 bytes are set to 0 
    string_count++;
    return str;
}

bool isConstant(void *str){
    char *first_byte = (char *)(str);
    char flag =first_byte[0];
    return flag!=0;
}

int getReferenceCount(void *str){
    char *p = (char *)(str);
    int * counter = (int *)(p+1);
    return *counter;
}

void freeString(void * str){
    free(str);
}

int myFunc(int t[],int size){
    printf("%d",t[0]);
    return 1;
}

// size is in bytes 
int8_t * allocateInstance(int size){
    // memset(arr+1,0,item_count*item_size);
    // return (int8_t *)malloc(size);
    int8_t *ptr = (int8_t *)malloc(size);
    memset(ptr,0,size);
    return ptr;
}

int8_t * getField(int8_t *object,int offset){
    return (object+offset);
}

}