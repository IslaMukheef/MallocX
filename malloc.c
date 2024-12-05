#include <malloc.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
// https://man7.org/linux/man-pages/man3/malloc.3.html
// https://stackoverflow.com/questions/381244/purpose-of-memory-alignment
#define ALIGNMENT 8 
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct MemoryNode {
  size_t size; //size of memeory address 
  struct MemoryNode *next;
  int is_free;  // flags 0= in use, 1= free to use
}MemoryNode;

MemoryNode *free_list = NULL; // head of list

void *malloc(size_t size){
  size =ALIGN(size); 
  
  //check pool for free Memeory to use
  MemoryNode *current = free_list; // set current to the head 
  while(current){
    if(current->is_free && current->size >=size) // is free =1 , current size > or = needed size
    {
      current->is_free = 0; // set it to be in use 
      return(void*)(current+1); // return memeory address of current 
    }
    current = current->next; // set current to next in list
  }

  //create node if non was found in pool
  MemoryNode *new_node = sbrk(size+sizeof(MemoryNode)); // man 2 sbrk 
  if(new_node == (void*)-1) return NULL; // sbrk failed 
  
  new_node->size = size; // set node size
  new_node->is_free = 0; // set flag to 0 for in use
  new_node->next = NULL; // set the new_node as the end of list
  //add new node to the memeory pool(free list)
  if(free_list){
    current = free_list;
    while(current->next) // add new_node to the end of the list
    {
      current=current->next;
    }
    current->next = new_node;
  }
  else{
    free_list = new_node; // if no list create one
  }

  return(void*)(new_node+1);
}

void free(void *ptr) // man 3 free
{
 if(!ptr) return; // return on null 
 MemoryNode *node_to_free = (MemoryNode*) ptr-1; //get the metadata not the memory addr 
 node_to_free->is_free = 1; // free to use

 MemoryNode *current = free_list; // current to head 
 while(current)
  {
    // current is free to use && there is next && next of current is free as well
    if (current->is_free && current->next && current->next->is_free) 
    {
      // Merge current and its next node
      current->size += sizeof(MemoryNode) + current->next->size;
      current->next = current->next->next;
    }
    else
    {
      current = current->next;
    }
  }
}


void *calloc(size_t nmemb, size_t size){
  size_t total_size = nmemb *size; // get total size of all elements
  
  //check for overflow
  if(nmemb != 0 && total_size/nmemb !=size) // no dividing on 0, total_size / nmemb should equal to size 
  {
    return NULL;
  }
  void *ptr = malloc(total_size); // use our malloc to create allocate MemoryNode
  if(!ptr){
    return NULL;
  }

  memset(ptr,0, total_size); //  fill memory with a zero // man 3 memset
  return ptr;
}


void *realloc(void *ptr, size_t size) // man 3 realloc
{
  if(!ptr) // if ptr = null call malloc to create it directly
  {
    return malloc(size);
  }
  if(size == 0) // free the ptr if there is no size
  {
   free(ptr);
   return NULL;
  }

  MemoryNode *current = (MemoryNode*) ptr-1; //get the metadata not the memory addr 
  if(current->size >=size) // check if the size is already met
  {
    return ptr;
  }
  
  void *new_ptr = malloc(size); // call our malloc 
  if(!new_ptr){
    return NULL; // on fail
  }
  memcpy(new_ptr, ptr, current->size); // man 3 memcpy 
  free(ptr); //free old ptr 
  return new_ptr;
}
