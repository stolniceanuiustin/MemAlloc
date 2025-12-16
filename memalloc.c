#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>



typedef char ALIGN[16];

typedef union header
{
    struct {
        size_t size;
        unsigned is_free;
        union header* next;
    } s;
    ALIGN stub;
}header_t;
header_t *get_free_block(size_t size);

header_t *head = NULL, *tail = NULL;

void *mymalloc(size_t size)
{
    if(!size)
        return NULL;
   
    size_t total_size;
    void *block;
    header_t *header;

    header = get_free_block(size);

    if(header)
    {
        header->s.is_free = 0;
        return (void*)(header+1);
    } 

    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if(block == (void*) -1)
    {
        return NULL;
    }
    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;
    
    if(!head)
        head = header;
    if(tail)
        tail->s.next = header;
    tail = header;
    return (void*)(header+1);
    // block = sbrk(size);
    // if(block == (void*) -1)
    //     return NULL;
    // else 
    //     return block;
}



header_t *get_free_block(size_t size)
{
    header_t* curr = head;
    while(curr)
    {
        if(curr->s.is_free && curr->s.size >= size)
            return curr;
        curr = curr -> s.next;
    }
    return NULL;
}

void free(void* block)
{
    header_t *header, *tmp;
    void *programbreak;
    
    if(!block)
        return;
    
    header = (header_t*)block - 1;
    programbreak = sbrk(0);
    if((char*)block + header->s.size == programbreak)
    {
        if(head == tail)
        {
            head = tail = NULL;
        }
        else
        {
            tmp = head;
            while(tmp)
            {
                if(tmp->s.next == tail)
                {
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }
        }
        sbrk(0 - sizeof(header_t) - header->s.size);
        return;
    }
    header->s.is_free = 1;

}

int main()
{
    int* v = mymalloc(1000*sizeof(int));
    int* t = mymalloc(1000*sizeof(int));
    for(int i=0; i<1000; i++)
    {
        printf("%d\n", v[i]);
    }
    free(t);
    free(v);
    
}