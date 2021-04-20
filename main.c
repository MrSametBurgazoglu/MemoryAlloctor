/* Created By: Samet Burgazoğlu
 * Creation Date: 18/04/2021
 * This library is implementation of malloc and free functions
 * */

#include <stdio.h>
#include <unistd.h>
#include "mymalloc.h"

Block* split(Block *b, size_t size){
    b->info.size -= size+sizeof(Block);/*set new size of old block*/
    Block *new_block =  (void*) b + b->info.size + sizeof(Block);/*pointer to beginning of new block*/
    new_block->info.size = size;
    new_block->info.isfree = 0;
    new_block->prev = (struct block *) b;/*old block is left block of new block*/
    new_block->next = b->next;/*new block's next is old block's next */
    if(b->next != NULL){
        Block *next_block = (Block *) b->next;
        next_block->prev = (struct block *) new_block;
    }
    b->next = (struct block *) new_block;/*new blcok is right blcok of old block */
    return new_block;
}

void add_to_free_list(Block *b){/* add b to free list*/
    Block *current_block = free_list;
    while (current_block->next_free != NULL) {
        current_block = (Block *) current_block->next_free;
    }
    current_block->next_free = (struct block *) b;/*last element of list become b*/
    b->prev_free = (struct block *) current_block;
}

void delete_from_free_list(Block *b){
    if(b->next_free != NULL && b->prev_free != NULL){/* delete from middle of the list */
        Block *temp_f = (Block *) b->next_free;
        Block *temp_b = (Block *) b->prev_free;
        temp_b->next_free = (struct block *) temp_f;
        temp_f->prev_free = (struct block *) temp_b;
    }
    else if(b->next_free != NULL){/* delete from beginning of the list*/
        Block *temp_f = (Block *) b->next_free;
        temp_f->prev_free = NULL;
    }
    else if(b->prev_free != NULL){/* delete from end of the list */
        Block *temp_b = (Block *) b->prev_free;
        temp_b->next_free = NULL;
    }
}

void* find_best_fit(size_t size){
    Block *current_block = free_list;
    Block *result = current_block;
    while(current_block != NULL){
        if(current_block->info.size > size+sizeof(Block) && (size - current_block->info.size) < (size - result->info.size)){/* if difference between current block size and wanted size is lesser than result then change the result block */
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
        if(current_block->info.size > size+sizeof(Block) && (size - current_block->info.size) > (size - result->info.size)){/* if difference between current block size and wanted size is bigger than result then change the result block */
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
        if(current_block->info.size > size+sizeof(Block)){/* find the first block have enough size */
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
        /* initialization of heap*/
        heap_start = sbrk(1024);/* take from heap */
        heap_start->info.size = 1024-sizeof(Block);
        heap_start->info.isfree = 1;
        heap_start->next = NULL;
        heap_start->prev = NULL;
        heap_start->next_free = NULL;
        heap_start->prev_free = NULL;
        first_call = 0;
        free_list = heap_start;
    }
    void *result;/* result block*/
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
    Block *b = (Block *) (p - sizeof(Block));/* take block*/
    b->info.isfree = 1;
    if(b->next != NULL) {/* if there is another block in front of the current block */
        Block *current_block = (Block *) b->next;/* next block */
        if (current_block->info.isfree) {/* merge it with current block if its free*/
            b->info.size += current_block->info.size + sizeof(Block);
            b->next = current_block->next;
            delete_from_free_list(current_block);
        }
    }
    if(b->prev != NULL) {/* if there is another block behind the current block. */
        Block *current_block = (Block *) b->prev;/* prev block */
        if (current_block->info.isfree) {/* merge it with current block if its free*/
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
    myfree(alan2);
    printHeap();
    brk(heap_start);
    return 0;
}
