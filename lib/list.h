#ifndef LIST_H_
#define LIST_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define checkMem(x) if(x==NULL){\
                        printf("Allocation failed , check memmory\n");\
                        return 1;\
                    }
#define fOpenToW(f,name)    if((f=fopen(name,"w"))==NULL) {\
                                printf("Cannot create file '%s'\n",name);\
                                return 1;\
                            }
#define fOpenToR(f,name)    if((f=fopen(name,"r"))==NULL) {\
                                printf("Cannot open file '%s'\n",name);\
                                return 1;\
                            }
typedef struct elementType_t {
    char string[256];
    unsigned long freq;      
}elementType;
typedef struct node_t {
    elementType element;
    struct node_t *next;
}node;
typedef struct list_t {
    node *root;
    int size;
}list;
node* newNode(elementType e);
list* newList();
int addNode(list *l,elementType e);
int addTail(list *l,elementType e);
node* delHead(list *l);
int isEmpty(list *l);
void freeList(list *l);
#endif//LIST_H_