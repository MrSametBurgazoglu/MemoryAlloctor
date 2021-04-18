#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "mymalloc.h"

Block* split(Block *b, size_t size){
    b->info.size -= size+sizeof(Block);
    Block *new_block =  (void*) b + b->info.size + sizeof(Block);
    new_block->info.size = size;
    new_block->info.isfree = 0;
    new_block->prev = (struct block *) b;
    new_block->next = b->next;
    b->next = (struct block *) new_block;
    return new_block;
}

void add_to_free_list(Block *b){
    Block *current_block = free_list;
    while (current_block->next_free != NULL) {
        current_block = (Block *) current_block->next_free;
    }
    current_block->next_free = (struct block *) b;
    b->prev_free = (struct block *) current_block;
}

void delete_from_free_list(Block *b){
    if(b->next_free != NULL && b->prev_free != NULL){
        Block *temp_f = (Block *) b->next_free;
        Block *temp_b = (Block *) b->prev_free;
        temp_b->next_free = (struct block *) temp_f;
        temp_f->prev_free = (struct block *) temp_b;
    }
    else if(b->next_free != NULL){
        Block *temp_f = (Block *) b->next_free;
        temp_f->prev_free = NULL;
    }
    else{
        Block *temp_b = (Block *) b->prev_free;
        temp_b->next_free = NULL;
    }
}

void* find_best_fit(size_t size){
    Block *current_block = free_list;
    Block *result = current_block;
    while(current_block != NULL){
        if(current_block->info.size > size+sizeof(Block) && (size - current_block->info.size) < (size - result->info.size)){
            result = current_block;
        }
        current_block = (Block *) current_block->next;
    }
    if(result->info.size > size+sizeof(Block)){/* if there is no block matching don't return first block  */
        Block *new_block = split(result, size);
        return (void*)new_block+sizeof(Block);
    }
    return NULL;
}

void* find_worst_fit(size_t size){
    Block *current_block = free_list;
    Block *result = current_block;
    while(current_block != NULL){
        if(current_block->info.size > size+sizeof(Block) && (size - current_block->info.size) > (size - result->info.size)){
            result = current_block;
        }
        current_block = (Block *) current_block->next;
    }
    if(result->info.size > size+sizeof(Block)){/* if there is no block matching don't return first block  */
        Block *new_block = split(result, size);
        return (void*)new_block+sizeof(Block);
    }
    return NULL;
}

void* find_first_fit(size_t size){
    Block *current_block = free_list;
    while(current_block != NULL){
        if(current_block->info.size > size+sizeof(Block)){
            Block *new_block = split(current_block, size);
            return (void*)new_block+sizeof(Block);
        }
        current_block = (Block *) current_block->next;
    }
    return NULL;
}

void* mymalloc(size_t size){
    size = (size / 16 + 1) * 16;
    static int first_call = 1;
    if(first_call){
        heap_start = sbrk(1024);
        heap_start->info.size = 1024-sizeof(Block);
        heap_start->info.isfree = 1;
        heap_start->next = NULL;
        heap_start->prev = NULL;
        heap_start->next_free = NULL;
        heap_start->prev_free = NULL;
        first_call = 0;
        free_list = heap_start;
    }
    void *result;
    switch (strategy) {
        case bestFit:
            result = find_best_fit(size);
            break;
        case worstFit:
            result = find_worst_fit(size);
            break;
        case firstFit:
            result = find_first_fit(size);
            break;
        default:
            result = NULL;
            break;
    }
    return result;
}

void* myfree(void *p) {
    Block *b = (Block *) (p - sizeof(Block));
    b->info.isfree = 1;
    if(b->next != NULL) {
        Block *current_block = (Block *) b->next;
        if (!current_block->info.isfree) {
            b->info.size += current_block->info.size + sizeof(Block);
            b->next = current_block->next;
            delete_from_free_list(current_block);
        }
    }
    if(b->prev != NULL) {
        Block *current_block = (Block *) b->prev;
        if (!current_block->info.isfree) {
            current_block->info.size += b->info.size + sizeof(Block);
            current_block->prev = b->prev;
            delete_from_free_list(current_block);
            b = current_block;
        }
    }
    add_to_free_list(b);
    return b;
}

void printHeap(){
    Block *current_block = heap_start;
    printf("Blocks:\n");
    while (current_block != NULL) {
        printf("Free: %d && ", current_block->info.isfree);
        printf("Size: %d\n", current_block->info.size);
        current_block = (Block *) current_block->next;
    }
}

int main() {
    void *alan = mymalloc(100);
    void *alan2 = mymalloc(200);
    void *alan3 = mymalloc(200);
    myfree(alan);
    printHeap();
    brk(heap_start);
    return 0;
}
