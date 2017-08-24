#ifndef HUFFMAN_H_
#define HUFFMAN_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#define MAX_LEN 32
#define MAX_SIZE 200000000
typedef struct {
    unsigned long freq;// index of character in huffman tree    
    char code[MAX_LEN];// code of this character
}character;
unsigned int indexOfCode(char *c) ;
char *codeOfIndex(unsigned int index);
void update(character *charac, char *s, char c);
int ghepNode(list *l,character *charac) ;
void reverse(char *s);
int enhuff(char *ifilename,char *ofilename);
int dehuff(char *iname,char *oname);
int saveHuffmanTable(unsigned int *indexA,char *charA,int numOfChar,char *filename);
#endif//HUFFMAN_H_
