//
// Created by samet on 13.04.2021.
//
#ifndef MYMALLOC_H
#define MYMALLOC_H

#include <stdio.h>

typedef enum {bestFit, worstFit, firstFit} Strategy;

Strategy strategy = bestFit;


typedef struct{
    unsigned int size;
    unsigned int isfree;
}Info;

typedef struct{
    Info info;           /*number of  16 byte blocks*/
    struct block *next;  /*next free*/
    struct block *prev;
    struct block *next_free;
    struct block *prev_free;
    char data[0];        /*start of the allocated memory*/
}Block;
/*Block: |4byte(size)|4byte(isfree)|8-byte next|0-byte(data)|*/

static Block *free_list = NULL;     /*start of the free list*/
static Block *heap_start = NULL;    /*head of allocated memory from sbrk */


void *mymalloc(size_t size);
void *myfree(void *p);
Block *split(Block *b, size_t size);
void printHeap();

#endif //MYMALLOC_H
