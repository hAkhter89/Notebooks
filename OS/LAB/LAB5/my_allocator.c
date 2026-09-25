#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct Block {
  size_t size;
  int free;
  struct Block *next;
} Block;

#define ALIGNMENT 16
#define CHUNK_SIZE 65536

Block *head = NULL;

/* Round size to a multiple of 16. */
size_t align_size(size_t size) {
  return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

size_t header_size(void) { return align_size(sizeof(Block)); }

/* Check whether two blocks are physically next to each other in memory. */
int adjacent(Block *a, Block *b) {
  return (char *)a + header_size() + a->size == (char *)b;
}

/* Ask the operating system for a new chunk of memory using mmap(). */
Block *request_memory(size_t size) {
  size_t total = header_size() + size;

  /* Get at least 64 KiB so we can reuse the memory for future allocations. */
  if (total < CHUNK_SIZE)
    total = CHUNK_SIZE;

  total = (total + 4095) & ~4095UL;

  void *memory = mmap(NULL, total, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (memory == MAP_FAILED)
    return NULL;

  /* The beginning of the mmap memory becomes block header. */
  Block *block = memory;

  block->size = total - header_size();
  block->free = 1;
  block->next = NULL;

  if (head == NULL) {
    head = block;
  } else {
    Block *current = head;

    while (current->next != NULL)
      current = current->next;

    current->next = block;
  }

  return block;
}

void split_block(Block *block, size_t size) {
  /*
   * Only split if the remaining space is large enough for
   * another header and some usable memory.
   */
  if (block->size < size + header_size() + ALIGNMENT)
    return;

  Block *new_block = (Block *)((char *)block + header_size() + size);

  new_block->size = block->size - size - header_size();
  new_block->free = 1;
  new_block->next = block->next;

  block->size = size;
  block->next = new_block;
}

void *my_malloc(size_t size) {
  if (size == 0)
    return NULL;

  size = align_size(size);

  // find the first free block that is large enough.
  Block *current = head;

  while (current != NULL) {
    if (current->free && current->size >= size)
      break;

    current = current->next;
  }

  /*
   * No suitable free block exists, so request more memory
   * from the operating system.
   */
  if (current == NULL) {
    current = request_memory(size);

    if (current == NULL)
      return NULL;
  }

  split_block(current, size);

  current->free = 0;

  /*
   * The user receives the memory after the header.
   */
  return (char *)current + header_size();
}

void my_free(void *ptr) {
  if (ptr == NULL)
    return;

  Block *block = (Block *)((char *)ptr - header_size());

  block->free = 1;

  while (block->next != NULL && block->next->free &&
         adjacent(block, block->next)) {

    block->size += header_size() + block->next->size;
    block->next = block->next->next;
  }

  Block *previous = NULL;
  Block *current = head;

  while (current != NULL && current != block) {
    previous = current;
    current = current->next;
  }

  if (previous != NULL && previous->free && adjacent(previous, block)) {

    previous->size += header_size() + block->size;
    previous->next = block->next;
  }
}

/* Print heap/list. */
void print_blocks(void) {
  Block *current = head;
  int i = 0;

  printf("\nHeap blocks:\n");

  while (current != NULL) {
    printf("Block %d: address=%p size=%zu %s\n", i, (void *)current,
           current->size, current->free ? "FREE" : "USED");

    current = current->next;
    i++;
  }
}

int main(void) {
  printf("Header size: %zu bytes\n", header_size());

  printf("\nAllocating memory...\n");

  char *a = my_malloc(32);
  char *b = my_malloc(100);
  char *c = my_malloc(200);

  strcpy(a, "Hello allocator!");
  strcpy(b, "This is block B.");
  strcpy(c, "This is block C.");

  printf("a: %s\n", a);
  printf("b: %s\n", b);
  printf("c: %s\n", c);

  print_blocks();

  printf("\nFreeing b\n");
  my_free(b);
  print_blocks();

  printf("\nFreeing a\n");
  my_free(a);
  print_blocks();

  printf("\nFreeing c\n");
  my_free(c);
  print_blocks();

  return 0;
}
