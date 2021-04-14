#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "mymalloc.h"
Block* split(Block *b, size_t size){
    b->info.size -= size+sizeof(Block);
    Block *new_block =  (void*) b + b->info.size + sizeof(Block);
    new_block->info.size = size;
    new_block->info.isfree = 0;
    b->next = (struct block *) new_block;
    new_block->prev = (struct block *) b;
    return new_block;
}

void* mymalloc(size_t size){
    static int first_call = 1;
    if(first_call){
        heap_start = sbrk(1024);
        heap_start->info.size = 1024-sizeof(Block);
        heap_start->info.isfree = 1;
        first_call = 0;
        free_list = heap_start;
    }
    Block *current_block = heap_start;
    while(current_block->next != NULL){
        if(current_block->info.isfree == 1 && current_block->info.size > size){
            Block *new_block = split(current_block, size);
            return (void*)new_block+sizeof(Block);
        }
        current_block = (Block *) current_block->next;
    }
    return NULL;
}

void* myfree(void *p){
    Block *b = (Block*) (p- sizeof(Block));
    b->info.isfree = 1;
    Block *current_block = (Block *) b->next;
    if(!current_block->info.isfree){
        b->info.size += current_block->info.size + sizeof(Block);
        b->next = current_block->next;
    }
    current_block = (Block *) b->prev;
    if(!current_block->info.isfree){
        current_block->info.size += b->info.size + sizeof(Block);
        current_block->prev = b->prev;
        b = current_block;
    }
    return (void *) b;
}

int main() {
    printf("Hello, World!\n");
    return 0;
}